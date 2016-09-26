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

void bheap_bubble_down(struct bheap*, size_t);
void bheap_bubble_up(struct bheap*);
void bheap_update(struct bheap*, struct pathloc*);

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

  while(bheap_peek(bheap))
    bheap_pop(bheap);

  free(bheap->nodes);
  free(bheap);
  *bheapp = NULL;
}

void
bheap_bubble_up(struct bheap* bheap)
{
  size_t my_idx, parent_idx;
  struct pathloc* parent, *me;

  my_idx = bheap->last_node -1;

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
}

void
bheap_bubble_down(struct bheap* bheap, size_t node_idx)
{
  struct pathloc *root, *child1, *child2, *tmp;

  while ((node_idx*2 +1) < bheap->last_node) {
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

  bheap->last_node++;
  bheap_bubble_up(bheap);
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
    bheap->nodes[bheap->last_node] = NULL;

    bheap_bubble_down(bheap, 1);
  }

  return min;
}

void
bheap_update(struct bheap* bheap, struct pathloc* node)
{
  size_t my_idx = 0;

  for (size_t i = 1; i < bheap->last_node; i++) {
    if (bheap->nodes[i] == node)
      my_idx = i;
  }

  if (my_idx == 0)
    return;

  bheap_bubble_down(bheap, my_idx);
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

static size_t storage_len = 0;
static struct pathloc** storage = NULL;
static void delete_storage(void);

static void
alloc_storage(const struct maze* maze)
{
  storage_len = maze->maze_width * maze->maze_height;

  // Each vertex is added into an array
  storage = calloc(storage_len, sizeof(*storage));

  // Add each space in the maze to the storage array
  for (uint16_t y = 0; y < maze->maze_height; y++) {
    for (uint16_t x = 0; x < maze->maze_width; x++) {

      struct location loc = (struct location){.x = x, .y = y};

      if (!maze_is_empty_space_loc(maze, loc))
        continue;

      struct pathloc* ploc = calloc(1, sizeof(*ploc));
      if (!ploc) exit(1);

      ploc->loc = loc;
      storage[y * maze->maze_width +x] = ploc;
    }
  }

  atexit(delete_storage);
}

static void
delete_storage(void)
{
  // Cleanup the storage
  for (uint16_t i = 0; i < storage_len; i++)
    free(storage[i]);
  free(storage);

}

// FIXME
// Don't implement the stack here
struct path*
path_find(const struct maze* maze, struct location source, struct location dest)
{
  // target is the pathloc of the target vertex
  struct pathloc* target = NULL;

  if (storage == NULL)
    alloc_storage(maze);

  if (!maze_is_empty_space_loc(maze, source) ||
      !maze_is_empty_space_loc(maze, dest))
      return NULL;

  struct bheap* bheap;
  bheap = bheap_new();

  for (uint16_t i = 0; i < storage_len; i++) {
    if (storage[i]) {
      storage[i]->distance = 0x10000; // some large number
      storage[i]->in_queue = false;
      storage[i]->visited = false;
      storage[i]->parent = NULL;
    }
  }

  // Initial is our current(starting) location
  {
    struct pathloc* initial;
    initial = storage[source.y * maze->maze_width + source.x];
    initial->loc = source;
    initial->distance = 0; // distance to self is 0
    initial->in_queue = true;
    bheap_insert(bheap, initial);
  }

  // Calculate the distance from the source to each node
  // This is the "main loop" of the pathfinder
  while (bheap_peek(bheap)) {
    struct pathloc *min = bheap_pop(bheap);
    min->visited = true;

    // Break when we find the target
    if (min->loc.x == dest.x && min->loc.y == dest.y) {
      target = min;
      break;
    }

    // Check each of the 4 adjacent locations for an empty space
    struct pathloc *adj[4] = { 0 };

    if (location_adjacent(min->loc,
          (struct location) {
            .x = min->loc.x -1,
            .y = min->loc.y
          }))
      adj[0] = storage[min->loc.y * maze->maze_width + min->loc.x - 1];

    if (location_adjacent(min->loc,
          (struct location) {
            .x = min->loc.x,
            .y = min->loc.y -1
          }))
      adj[1] = storage[(min->loc.y - 1) * maze->maze_width + min->loc.x];

    if (location_adjacent(min->loc,
          (struct location) {
            .x = min->loc.x +1,
            .y = min->loc.y
          }))
      adj[2] = storage[min->loc.y * maze->maze_width + min->loc.x + 1];

    if (location_adjacent(min->loc,
          (struct location) {
            .x = min->loc.x,
            .y = min->loc.y +1
          }))
      adj[3] = storage[(min->loc.y +1) * maze->maze_width + min->loc.x];

    // If a space is empty, try to update its distance
    for (size_t i = 0; i < 4; i++) {
      if (!adj[i]) continue;

      if (adj[i]->distance > min->distance+1) {
        if (adj[i]->in_queue == false) {
          bheap_insert(bheap, adj[i]);
          adj[i]->in_queue = true;
        }
        adj[i]->distance = min->distance+1;
        adj[i]->parent = min;

        bheap_update(bheap, adj[i]);
      }
    }
  }

  // Find the path.
  // We're essentially back tracing thru the path.
  // So we use a stack to reverse the direction
  // The last move required to get us to the target goes down first, to the
  // bottom of the stack.
  struct path* ret_path = calloc(1, sizeof(*ret_path));
  if (!ret_path)
    exit(1);

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

    while (ret_path->next < ret_path->num_steps && stack_top > 0)
      ret_path->steps[ret_path->next++] = stack[--stack_top];
    free(stack);

    ret_path->next = 0;
  } else {
    free(ret_path);
    ret_path = NULL;
  }

  // Cleanup the allocated memory for the heap
  bheap_delete(&bheap);

  return ret_path;
}
