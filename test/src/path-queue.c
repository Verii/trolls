#include "path.h"
#include <stdlib.h>

/* pathloc is used by Dijkstra's algorithm below,
 * it hold the location as an (x, y) coord pair,
 * The iterative distance to that node (initally infinity)
 * And a link to that node's parent
 */
struct pathloc
{
  struct location loc;
  int32_t distance;
  struct pathloc* parent; // links for the path
};

/* queue is a double linked list
 *  - node points to the pathloc (defined above)
 */
struct queue
{
  struct pathloc* node;
  struct queue *next, *prev;
};

// Return an array of steps to get from source location (s) to target location
// (t). The length of the array is returned in the passes size_t pointer (l).
//
// Calculate the shortest route to the destination (dest)
//
// Returns non-zero on all of the following: &&
//  - a path structure was found for this troll
//  - the destination is valid and within range
//  - an actual path was found to reach the destination
//
// Returns zero on any of the following: ||
//  - no path structure has been allocated for this troll
//  - destination is invalid
//  - we cannot calculate a path to the destination
//
////////////////////
////////////////////
//
// - Mark each node with a distance of infinity(-1) and being unvisited
// - Each node has an undefined parent node
// - Check if the node is a traversible space - drop it if we can't move there
// - Add all unvisited nodes to a queue
//
// While queue is not empty:
//  cur = find a node with the smallest distance
//
//  if cur == target
//   terminate while loop
//
//  foreach adjacent node(a) to cur(c):
//   new_dist = distance of c + distance between c and a
//   if new_dist < distance of a
//    distance of a = new_dist
//    parent of a = c
//
//  remove cur from queue
//
// Start with target node, and follow the prev(parent) links up to the
// current node. This is the reverse direction we need to travel to reach the
// target node.
//
// Push each node onto a stack as we're following their parent.
// Pop each node from the stack and calculate the direction we need to travel
//  to get there.
// Then append the direction to the steps array (up to MAX_STEPS moves).

// FIXME
// Replace the double linked list queue with a minimum priority queue
// FIXME
// Don't add all nodes to the queue, only add the child nodes
// FIXME
// Don't implement the stack here
struct path*
path_find(const struct maze* maze, struct location source, struct location dest)
{
  struct path* ret_path = calloc(1, sizeof(*ret_path));
  if (!ret_path)
    exit(1);

  // target is the pastloc of the target vertex
  struct pathloc* target = NULL;

  // Each vertex is added into an array (for storage, so we can delete it
  // later)
  size_t storage_idx = 0;
  struct pathloc** storage;
  storage = calloc(maze->maze_width * maze->maze_height, sizeof(*storage));

  // ALSO put each pathloc in a queue(double linked list)
  struct queue* head;
  head = calloc(1, sizeof(*head));

  // first vertex is the source
  head->node = calloc(1, sizeof(*(head->node)));
  if (!head->node)
    exit(1);

  head->node->loc = source;
  head->node->distance = 0; // Distance to self is 0
  storage[storage_idx++] = head->node;

  /* Find each valid space and add it to the queue */
  for (uint16_t y = 0; y < maze->maze_height; y++) {
    for (uint16_t x = 0; x < maze->maze_width; x++) {
      struct location loc = (struct location){
        .x = x, .y = y,
      };

      if (!maze_is_empty_space_loc(maze, loc) ||
          // ignore the source location since we added that one above
          (loc.x == source.x && loc.y == source.y))
        continue;

      struct queue* new_head = calloc(1, sizeof(*new_head));
      if (!new_head)
        exit(1);

      struct pathloc* ploc = calloc(1, sizeof(*ploc));
      if (!ploc)
        exit(1);

      ploc->loc = loc;
      ploc->distance = -1;

      new_head->node = ploc;

      // Insert the new location to the head of the queue, and into the storage
      // array
      new_head->next = head;
      head = new_head;
      head->next->prev = head;
      storage[storage_idx++] = head->node;
    }
  }

  // Calculate the distance from the source to each node
  // This is the "main loop" of the pathfinder
  while (head) {

    // Find the vertex with the smallest distance
    int32_t smallest_distance = -1;
    struct queue* current = NULL;

    // A distance of -1 == infinity, so we can't purely check which node has
    // the smallest distance.
    // First we find the node with a distance >= 0
    // Then we find the node with the smallest distance (excluding those < 0)
    for (struct queue* smallest = head; smallest; smallest = smallest->next) {
      // initially find the first non-zero distance node
      if (smallest_distance == -1 && smallest->node->distance >= 0) {
        smallest_distance = smallest->node->distance;
        current = smallest;
        continue;
      }

      // Then compare each remaining node to the current smallest
      // current will eventually point to the smallest node after visiting
      // every element
      if (smallest->node->distance >= 0 && // negative distance == infinity
          smallest->node->distance < smallest_distance) {

        smallest_distance = smallest->node->distance;
        current = smallest;
      }
    }

    // Break when we find the target
    if (current->node->loc.x == dest.x && current->node->loc.y == dest.y) {
      target = current->node;
      break;
    }

    // Find adjacent nodes and update their distances
    for (struct queue* adj = head; adj; adj = adj->next) {
      // only update adjacent nodes
      if (!location_adjacent(current->node->loc, adj->node->loc))
        continue;

      // Set the distance of the adjacent node to the current node's distance+1
      if (adj->node->distance == -1 ||
          adj->node->distance > current->node->distance + 1) {
        adj->node->distance = current->node->distance + 1;
      }

      // Set the adjacent node's parent to the current node
      adj->node->parent = current->node;
    }

    // Remove current from the list
    struct queue *next, *prev;
    prev = current->prev;
    next = current->next;

    if (prev && next) {
      prev->next = next;
      next->prev = prev;
    }

    if (prev && !next) {
      prev->next = next;
    }

    if (next && !prev) {
      next->prev = prev;
      head = next;
    }

    if (!next && !prev) {
      head = NULL;
    }
    free(current);
  }

  ret_path->next = 0;
  ret_path->num_steps = maze->maze_width * maze->maze_height;
  ret_path->steps = calloc(ret_path->num_steps, sizeof(*ret_path->steps));

  // Find the path.
  // We're essentially back tracing thru the path.
  // So we use a stack to reverse the direction
  // The last move required to get us to the target goes down first, to the
  // bottom of the stack.
  {
    enum direction* stack;
    stack = calloc(maze->maze_width * maze->maze_height, sizeof(*stack));
    size_t stack_top = 0;

    while (target && target->parent) {

      enum direction rel_dir = NORTH;
      location_relative(target->parent->loc, target->loc, &rel_dir);
      stack[stack_top++] = rel_dir;

      target = target->parent;
    }

    ret_path->num_steps = stack_top - 1;

    while (ret_path->next < ret_path->num_steps && stack_top > 0)
      ret_path->steps[ret_path->next++] = stack[--stack_top];
    free(stack);

    ret_path->next = 0;
  }

  // Cleanup the allocated memory for the queue and storage array
  //
  {
    while (head) {
      struct queue* tmp = head;
      if (tmp->next)
        tmp->next->prev = NULL;
      head = tmp->next;
      free(tmp);
    }

    // Cleanup the storage
    for (uint16_t i = 0; i < storage_idx; i++)
      free(storage[i]);
    free(storage);
  }

  return ret_path;
}
