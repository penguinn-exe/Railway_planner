#ifndef REVIEW_HPP
#define REVIEW_HPP

#include <string>
#include <vector>

struct Review {
    int reviewId;
    std::string targetType; // "TRAIN" or "STATION"
    std::string targetCode; // Train Number as string (e.g., "12951") or Station Code ("CSMT")
    std::string author;
    int rating; // 1 to 5
    std::string date;
    std::string comment;
    std::vector<std::string> tags;

    Review() : reviewId(0), rating(5) {}

    Review(int id, std::string type, std::string code, std::string author_, int rating_, std::string date_, std::string comment_, std::vector<std::string> tags_ = {})
        : reviewId(id), targetType(std::move(type)), targetCode(std::move(code)), author(std::move(author_)),
          rating(rating_), date(std::move(date_)), comment(std::move(comment_)), tags(std::move(tags_)) {}
};

#endif // REVIEW_HPP
