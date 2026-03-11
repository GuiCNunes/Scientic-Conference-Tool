#ifndef DATATYPES_H
#define DATATYPES_H

#include <string>

using namespace std;

struct Submission {
    int id;
    string title;
    string authors;
    string email;
    int primaryTopic;
    int secondaryTopic;
};

struct Reviewer {
    int id;
    string name;
    string email;
    int primaryExpertise;
    int secondaryExpertise;
};

struct ConfigParams {
    int minReviewsPerSubmission;
    int maxReviewsPerReviewer;
    int generateAssignments;
    int riskAnalysis;
};

struct Control {
    int generateAssignments;
    int riskAnalysis;
    string outputFilename = "output.csv";
};

#endif //DATATYPES_H
