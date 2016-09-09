#include "path.h"


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

enum direction *
path_find(const struct maze* m, struct location s, struct location t, size_t *l)
{
  struct pathloc
  {
    struct location loc;
    int32_t distance;
    struct pathloc* parent; // links for the path
  };

  struct queue
  {
    struct pathloc* node;
    struct queue *next, *prev;
  };

  struct pathloc** storage;
  struct queue* head;
  size_t vertex = 0;

  // Each vertex is added into an array (for storage, so we can delete it
  // later)
  storage = calloc(maze->maze_width * maze->maze_height, sizeof(*storage));

  // ALSO put each pathloc in a queue(double linked list)
  head = calloc(1, sizeof(*head));

  // first vertex is us, the troll
  head->node = calloc(1, sizeof(**storage));
  if (!head->node)
    exit(1);

  head->node->loc = troll->loc;
  head->node->distance = 0;
  storage[vertex++] = head->node;

  /* Find each valid space and add it to the queue */
  for (uint16_t y = 0; y < maze->maze_height; y++) {
    for (uint16_t x = 0; x < maze->maze_width; x++) {
      struct location loc = (struct location){
        .x = x, .y = y,
      };

      if (!maze_is_empty_space_loc(maze, loc) ||
          // ignore the troll location since we added that one above
          (loc.x == troll->loc.x && loc.y == troll->loc.y))
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
      storage[vertex++] = head->node;
    }
  }

  // Calculate the total distance of each node
  // This is the "main loop" of the pathfinder
  struct queue* current;
  while (head) {

    // Find the vertex with the smallest distance
    int32_t smallest_distance = -1;
    current = NULL;

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
    if (current->node->loc.x == dest.x && current->node->loc.y == dest.y)
      break;

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
    current = NULL;
  }

  // Save the target for later
  struct pathloc* target = NULL;
  if (current)
    target = current->node;
  current = NULL;

  // cleanup the queue
  while (head) {
    struct queue* tmp = head;
    if (tmp->next)
      tmp->next->prev = NULL;
    head = tmp->next;
    free(tmp);
  }
  head = NULL;

  // Find the path.
  // We're essentially back tracing thru the path.
  // So we use a stack to reverse the direction
  // The last move required to get us to the target goes down first, to the
  // bottom of the stack.
  // Then when we give the directions to the troll, we pop them off in reverse
  // order.
  //
  enum direction* stack;
  stack = calloc(maze->maze_width * maze->maze_height, sizeof(*stack));
  size_t stack_top = 0;

  while (target && target->parent) {

    enum direction rel_dir = NORTH;
    location_relative(target->parent->loc, target->loc, &rel_dir);
    stack[stack_top++] = rel_dir;

    target = target->parent;
  }

  while (troll_path->path.next < MAX_STEPS && stack_top > 0)
    troll_path->path.steps[troll_path->path.next++] = stack[--stack_top];

  free(stack);

  // Cleanup the storage
  for (uint16_t i = 0; i < vertex; i++)
    free(storage[i]);
  free(storage);
  vertex = 0;
  storage = NULL;

  troll_path->path.next = 0;
  troll_path->path.defined = true;

  return 1;
}
