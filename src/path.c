#include "path.h"
#include <stdlib.h>
#include <stdio.h>

/* pathloc is used by Dijkstra's algorithm below,
 * it hold the location as an (x, y) coord pair,
 * The iterative distance to that node (initally infinity)
 * And a link to that node's parent
 */
struct pathloc
{
  struct location loc;
  int32_t distance;
  bool visited;
  bool in_queue;
  struct pathloc* parent; // links for the path
};

struct bheap {
  size_t last_node;
  size_t length;
  struct pathloc **nodes;
};

struct bheap* bheap_new(void);
void bheap_delete(struct bheap**);
void bheap_insert(struct bheap*, struct pathloc*);
struct pathloc* bheap_peek(struct bheap*);
struct pathloc* bheap_pop(struct bheap*);


///
///
///


struct bheap*
bheap_new(void)
{
  struct bheap* bheap = calloc(1, sizeof(*bheap));
  if (!bheap) exit(1);

  bheap->last_node = 1;
  bheap->length = 512;
  bheap->nodes = calloc(bheap->length, sizeof(*bheap->nodes));
  if (!bheap->nodes) exit(1);

  return bheap;
}

void
bheap_delete(struct bheap** bheapp)
{
  struct bheap* bheap = *bheapp;
  free(bheap->nodes);
  free(bheap);
  *bheapp = NULL;
}

void
bheap_insert(struct bheap* bheap, struct pathloc* node)
{
  if (bheap->last_node >= bheap->length-1) {
    struct pathloc** new_nodes = realloc(bheap->nodes, sizeof(*bheap->nodes) * bheap->length*2);
    if (!new_nodes)
      return;
    bheap->length *= 2;
    bheap->nodes = new_nodes;
  }

  bheap->nodes[bheap->last_node] = node;

  if (bheap->last_node == 1) {
    bheap->last_node++;
    return;
  }

  size_t my_idx, parent_idx;
  struct pathloc* parent, *me;

  my_idx = bheap->last_node;

  while (my_idx > 1) {
    parent_idx = my_idx/2;
    parent = bheap->nodes[parent_idx];
    me = bheap->nodes[my_idx];

    if (parent->distance <= me->distance)
      break;

    bheap->nodes[parent_idx] = me;
    bheap->nodes[my_idx] = parent;

    my_idx /= 2;
  }

  bheap->last_node++;
}

struct pathloc *
bheap_peek(struct bheap* bheap)
{
  if (bheap->last_node < 2)
    return NULL;
  return bheap->nodes[1];
}

struct pathloc *
bheap_pop(struct bheap* bheap)
{
  if (bheap->last_node < 2)
    return NULL;

  struct pathloc* min = bheap->nodes[1];

  bheap->last_node--;

  if (bheap->last_node > 1) {
    bheap->nodes[1] = bheap->nodes[bheap->last_node];

    size_t node_idx = 1;
    struct pathloc *root, *child1, *child2, *tmp;

    while (1) {
      root = bheap->nodes[node_idx];
      child1 = bheap->nodes[node_idx *2];
      child2 = bheap->nodes[node_idx *2 +1];

      if (child1 == NULL)
        break;

      if (child2 == NULL) {
        if (root->distance <= child1->distance)
          break;
        bheap->nodes[node_idx] = child1;
        bheap->nodes[node_idx*2 +1] = root;
        break;
      }

      if (root->distance <= child1->distance &&
          root->distance <= child2->distance)
        break;

      size_t new_node_idx;

      tmp = child1;
      new_node_idx = node_idx *2;
      if (child1->distance > child2->distance) {
        tmp = child2;
        new_node_idx = node_idx *2 +1;
      }

      bheap->nodes[node_idx] = tmp;
      bheap->nodes[new_node_idx] = root;

      node_idx = new_node_idx;
    }
  }

  return min;
}

///
///

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
// Don't implement the stack here
struct path*
path_find(const struct maze* maze, struct location source, struct location dest)
{
  struct path* ret_path = calloc(1, sizeof(*ret_path));
  if (!ret_path) exit(1);

  // target is the pathloc of the target vertex
  struct pathloc* target = NULL;

  struct bheap* bheap;
  bheap = bheap_new();

  // Each vertex is added into an array
  size_t storage_idx = 0;
  struct pathloc** storage;
  storage = calloc(maze->maze_width * maze->maze_height, sizeof(*storage));

  // first vertex is the source
  {
    struct pathloc* initial = calloc(1, sizeof(*initial));
    if (!initial) exit(1);
    initial->loc = source;
    initial->distance = 0; // Distance to self is 0
    initial->in_queue = true;
    bheap_insert(bheap, initial);
    storage[storage_idx++] = initial;
    fprintf(stderr, "Added Source\n");
  }

  /* Find each valid space and add it to the storage array */
  for (uint16_t y = 0; y < maze->maze_height; y++) {
    for (uint16_t x = 0; x < maze->maze_width; x++) {

      struct location loc = (struct location){.x = x, .y = y};

      if (!maze_is_empty_space_loc(maze, loc) ||
          // ignore the source location since we added that one above
          (loc.x == source.x && loc.y == source.y))
        continue;

      struct pathloc* ploc = calloc(1, sizeof(*ploc));
      if (!ploc) exit(1);

      ploc->loc = loc;
      ploc->distance = 0x10000; // some large number
      storage[storage_idx++] = ploc;
    }
  }
  fprintf(stderr, "Added Others\n");

  // Calculate the distance from the source to each node
  // This is the "main loop" of the pathfinder
  while (bheap_peek(bheap)) {
    struct pathloc *min = bheap_pop(bheap);
    min->visited = true;

    fprintf(stderr, "Found minimum: %d\n", min->distance);

    // Break when we find the target
    if (min->loc.x == dest.x && min->loc.y == dest.y) {
      fprintf(stderr, "Found target\n");
      target = min;
      break;
    }

    struct pathloc *adj;

    // Find an adjacent space in our storage array
    for (uint16_t i = 0; i < storage_idx; i++) {
      adj = storage[i];

      if (adj->visited) {
        fprintf(stderr, "Node already visited %d/%d\n", i, storage_idx-1);
        continue;
      }

      if (!location_adjacent(min->loc, adj->loc)) {
        fprintf(stderr, "Node not adjacent %d/%d\n", i, storage_idx-1);
        continue;
      }

      fprintf(stderr, "(%d, %d) found neighbor (%d, %d)\n",
          min->loc.x, min->loc.y, adj->loc.x, adj->loc.y);

      if (adj->distance > min->distance+1) {
        if (adj->in_queue == false) {
          bheap_insert(bheap, adj);
          adj->in_queue = true;
        }
        adj->distance = min->distance+1;
        adj->parent = min;
        fprintf(stderr, "Updated Neighbor: %d\n", adj->distance);
      }
    }
  }

  // Find the path.
  // We're essentially back tracing thru the path.
  // So we use a stack to reverse the direction
  // The last move required to get us to the target goes down first, to the
  // bottom of the stack.
  if (target) {
    ret_path->next = 0;
    ret_path->num_steps = maze->maze_width * maze->maze_height;
    ret_path->steps = calloc(ret_path->num_steps, sizeof(*ret_path->steps));

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
    fprintf(stderr, "%lu steps to destination\n", ret_path->num_steps);

    while (ret_path->next < ret_path->num_steps && stack_top > 0)
      ret_path->steps[ret_path->next++] = stack[--stack_top];
    free(stack);

    ret_path->next = 0;
  } else {
    fprintf(stderr, "Could not find path (%d, %d) -> (%d, %d)\n", source.x, source.y, dest.x, dest.y);
    free(ret_path);
    ret_path = NULL;
  }

  // Cleanup the allocated memory for the heap
  while (bheap_peek(bheap))
    bheap_pop(bheap);
  bheap_delete(&bheap);

  // Cleanup the storage
  for (uint16_t i = 0; i < storage_idx; i++)
    free(storage[i]);
  free(storage);

  fprintf(stderr, "\n\n");
  return ret_path;
}
