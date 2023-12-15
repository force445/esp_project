#include <Arduino.h>

#pragma once
namespace Eloquent {
    namespace ML {
        namespace Port {
            class LogisticRegression {
                public:
                    /**
                    * Predict class for features vector
                    */
                    int predict(float *x) {
                        float votes[8] = { 0.0f };
                        votes[0] = dot(x,   -0.252061898245  , 0.109712447171  , 0.145782761034 );
                        votes[1] = dot(x,   -0.11161064993  , -0.004441849411  , 0.160572589063 );
                        votes[2] = dot(x,   0.253408759936  , -0.111743349183  , -0.090885443051 );
                        votes[3] = dot(x,   0.221314316886  , 0.282195264686  , -0.495864031042 );
                        votes[4] = dot(x,   -0.245372314017  , -0.114081723784  , 0.34736126184 );
                        votes[5] = dot(x,   0.148349014425  , -0.316607735215  , 0.200251309322 );
                        votes[6] = dot(x,   -0.418247938405  , 0.297198736661  , 0.033778510179 );
                        votes[7] = dot(x,   0.40422070935  , -0.142231790925  , -0.300996957344 );
                        // return argmax of votes
                        uint8_t classIdx = 0;
                        float maxVotes = votes[0];

                        for (uint8_t i = 1; i < 8; i++) {
                            if (votes[i] > maxVotes) {
                                classIdx = i;
                                maxVotes = votes[i];
                            }
                        }

                        return classIdx;
                    }

                protected:
                    /**
                    * Compute dot product
                    */
                    float dot(float *x, ...) {
                        va_list w;
                        va_start(w, 3);
                        float dot = 0.0;

                        for (uint16_t i = 0; i < 3; i++) {
                            const float wi = va_arg(w, double);
                            dot += x[i] * wi;
                        }

                        return dot;
                    }
                };
            }
        }
    }



