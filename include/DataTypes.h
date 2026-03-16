#ifndef DATATYPES_H
#define DATATYPES_H

#include <string>

using namespace std;

struct Submission {
    int id = -1; string title;
    string authors;
    string email;
    int primaryTopic = -1;
    int secondaryTopic = -1;
};

struct Reviewer {
    int id = -1;
    string name;
    string email;
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

struct Control {
    int generateAssignments = 1;
    int riskAnalysis = 0;
    string outputFilename = "output.csv";
};

#endif //DATATYPES_H
