#ifndef DATATYPES_H
#define DATATYPES_H

#include <string>

struct Submission {
    int id = -1;
    std::string title;
    std::string authors;
    std::string email;
    int primaryTopic = -1;
    int secondaryTopic = -1;
};

struct Reviewer {
    int id = -1;
    std::string name;
    std::string email;
    int primaryExpertise = -1;
    int secondaryExpertise = -1;
};

struct Parameters {
    int minReviewsPerSubmission = 1;
    int maxReviewsPerReviewer = 1;
    int primaryReviewerExpertise = 1;
    int secondaryReviewerExpertise = 0;
    int primarySubmissionDomain = 1;
    int secondarySubmissionDomain = 0;
};


enum class AssignmentMode {
    REGULAR  = 0,
    PRIMARY = 1,
    MIXED   = 2,
    FULL    = 3
};

struct Control {
    AssignmentMode generateAssignments = AssignmentMode::PRIMARY;
    int riskAnalysis = 0;
    std::string outputFilename = "output.csv";
};

#endif // DATATYPES_H
