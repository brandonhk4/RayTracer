#ifndef BVH_H
#define BVH_H

#include "bbox.h"
#include "hittable.h"
#include "hittable_list.h"

#include <vector>
#include <functional>
#include <iterator>
#include <unordered_map>

// LOOK AT THE LINK IN THE GOOGLE DOC
// TODO: Make a Linear BVH or a Hybrid BVH


class split_plane {
    public:
        int axis;
        float position;
        bool min = false;
        int left_count = 0;
        int right_count = 0;

        split_plane() {}

        split_plane(float position, int axis, bool min) : 
                    position(position),
                    axis(axis),
                    min(min) {}

        split_plane(const split_plane& sp) :
            position(sp.position),
            axis(sp.axis),
            min(sp.min),
            left_count(sp.left_count),
            right_count(sp.right_count) {}

        bool operator<(const split_plane& sp) const {
            return position < sp.position;
        }
        
        split_plane& operator=(const split_plane& sp) {
            axis = sp.axis;
            position = sp.position;
            min = sp.min;
            left_count = sp.left_count;
            right_count = sp.right_count;
            return *this;
        }
};

bool compareX(shared_ptr<hittable> a, shared_ptr<hittable> b) {
    return a->bounding_box()[0].min < b->bounding_box()[0].min;
}

bool compareY(shared_ptr<hittable> a, shared_ptr<hittable> b) {
    return a->bounding_box()[1].min < b->bounding_box()[1].min;
}

bool compareZ(shared_ptr<hittable> a, shared_ptr<hittable> b) {
    return a->bounding_box()[2].min < b->bounding_box()[2].min;
}

using compare_func = bool(*)(shared_ptr<hittable>, shared_ptr<hittable>);

class bvh_node : public hittable {
    private:
        static compare_func comparators[3];

        split_plane find_best_split_plane(std::vector<shared_ptr<hittable>>& objects, int start, int end) {
            float min_sam = FLT_MAX;
            split_plane best_plane;
            for (int axis = 0; axis < 3; ++axis) {
                std::vector<split_plane> candidates;
                candidates.reserve((end - start) * 2);
                for (auto it = objects.begin() + start; it != objects.begin() + end; ++it) {
                    interval& obox = (*it)->bounding_box()[axis];
                    // std::cout << "(" << obox.min << ", " << obox.max << ") | ";
                    split_plane p0(obox.min, axis, true);
                    split_plane p1(obox.max, axis, false);
                    
                    candidates.push_back(p0);
                    candidates.push_back(p1);
                }

                std::sort(candidates.begin(), candidates.end());

                int leftC = 0;
                int rightC = end - start;
                bool m = false;
                for (split_plane plane : candidates) {
                    plane.left_count = leftC += int(m);
                    m = plane.min;
                    plane.right_count = rightC -= int(!m);
                    float sam = plane.left_count * bound_box.left_area(axis, plane.position) + 
                                plane.right_count * bound_box.right_area(axis, plane.position);
        
                    if (sam < min_sam) {
                        min_sam = sam;
                        best_plane = plane;
                    }
                }
            }
            std::sort(objects.begin() + start, objects.begin() + end, comparators[best_plane.axis]);

            return best_plane;
        }

        shared_ptr<hittable> left;
        shared_ptr<hittable> right;
        bbox bound_box;
        bool leaf;

    public:
        bvh_node(hittable_list list) : bvh_node(list.objects, 0, list.objects.size()) {
            std::cout << "BVH Tree successfully constructed\n";
        }

        bvh_node(std::vector<shared_ptr<hittable>>& objects, int start, int end) {
            // std::cout << "Constructing (" << start << ", " << end << ")\n";
            for (auto it = objects.begin() + start; it != objects.begin() + end; ++it)
                bound_box = bbox(bound_box, (*it)->bounding_box());
            if (end - start <= 1) {
                left = right = objects[start];
                leaf = true;
            }
            else if (end - start == 2) {
                left = objects[start];
                right = objects[start + 1];
                leaf = true;
            } else {
                split_plane best_plane = find_best_split_plane(objects, start, end);
                int mid = best_plane.left_count + start;
                if (mid == end || mid == start) mid = (start + end) / 2;
                left = make_shared<bvh_node>(objects, start, mid);
                right = make_shared<bvh_node>(objects, mid, end);
            }

            bound_box = bbox(left->bounding_box(), right->bounding_box());
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            if (!bound_box.hit(r, ray_t)) return false;

            bool hit_left = left->hit(r, ray_t, rec);
            bool hit_right = right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

            return hit_left || hit_right;
        }

        bbox bounding_box() const override { return bound_box; }

        shared_ptr<hittable> left_object() const { return left; }

        shared_ptr<hittable> right_object() const { return right; }

        bool is_leaf() const { return leaf; }
};

compare_func bvh_node::comparators[] = {&compareX, &compareY, &compareZ};

// class bvh_array_node {
//     public:
//     bool leaf = false;
//     int left, right;
//     bbox bound_box;

//     bvh_array_node(const std::unordered_map<hittable*, int>& objects, bvh_node* node) {
//         if (node->is_leaf()) {
//             left = objects.at(node->left_object().get());
//             right = objects.at(node->right_object().get());
//             leaf = true;
//         }
//     }

//     bool hit(const ray& r, interval ray_t, hit_record& rec) const {
//         if (!bound_box.hit(r, ray_t)) return false;
//     }
// };


// // Make the tree contiguous in memory by using an array
// class bvh_tree : public hittable {
//     private:
//         bvh_node root;
//         std::vector<bvh_array_node> bvh_array;
//         std::unordered_map<hittable*, int> objects_map;
//         std::vector<shared_ptr<hittable>>& objects;

//         int tree_to_array(bvh_node* node) {
//             int index = bvh_array.size();
//             bvh_array.emplace_back(objects_map, node);
//             std::cout << node->left_object().get() << ", " << dynamic_cast<bvh_node*>(node->right_object().get()) << ", " << node << ", " << index << '\n';
//             bvh_array_node& a_node = bvh_array[index];
//             if (!a_node.leaf) {
//                 // bvh_node* left = (static_cast<bvh_node*>(node->left_object().get()));
//                 // std::cout << left->bounding_box()[0].min << ", " << left->bounding_box()[0].max << ", " << (static_cast<bvh_node*>(node->left_object().get())) << '\n';
//                 a_node.left = tree_to_array((static_cast<bvh_node*>(node->left_object().get())));
//                 a_node.right = tree_to_array((static_cast<bvh_node*>(node->right_object().get())));
//             }
//             std::cout << a_node.left << ", " << a_node.right << "\n";
//             return index;
//         }

//     public:
//         bvh_tree(hittable_list list) : root(list.objects, 0, list.objects.size()), objects(list.objects) {
//             for (int i = 0; i < objects.size(); ++i) {
//                 objects_map[objects[i].get()] = i;
//             }

//             tree_to_array(&root);

//             for (bvh_array_node& node : bvh_array) {
//                 std::cout << node.left << ", " << node.right << "\n";
//             }
//         }

//         bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
//             std::stack<bvh_array_node> stack;
//             stack.push(bvh_array[0]);
//             bool hit = false;
//             while (!stack.empty()) {
//                 bvh_array_node node = stack.top();
//                 stack.pop();
//                 if (node.bound_box.hit(r, ray_t)) {
//                     if (!node.leaf) {
//                         stack.push(bvh_array[node.right]);
//                         stack.push(bvh_array[node.left]);
//                     } else {
//                         bool left_hit = objects[node.left]->hit(r, ray_t, rec);
//                         if (left_hit) ray_t.max = rec.t;
//                         bool right_hit = objects[node.right]->hit(r, ray_t, rec);
//                         hit = hit || left_hit || right_hit;
//                     }
//                 }
//             }
//             return hit;
//         };

//         bbox bounding_box() const { return bvh_array[0].bound_box; }
// };

#endif