#ifndef INTERVAL_H
#define INTERVAL_H

class interval {
    public:
        float min, max;

        interval() : min(+infinity), max(-infinity) {}

        interval(float min, float max) : min(min), max(max) {}

        interval(const interval& a, const interval& b) : 
            min(std::min(a.min, b.min)),
            max(std::max(a.max, b.max)) {}

        interval(const interval& i) :
            min(i.min),
            max(i.max) {}

        float size() const {
            return max - min;
        }

        bool contains(float x) const {
            return x >= min && x <= max;
        }

        bool surrounds(float x) const {
            return x > min && x < max;
        }

        float clamp(float x) const {
            if (x < min) return min;
            if (x > max) return max;
            return x;
        }

        static const interval empty, universe;
};

const interval interval::empty = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);

#endif