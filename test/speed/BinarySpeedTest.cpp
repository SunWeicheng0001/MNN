//
//  BinarySpeedTest.cpp
//  MNNTests
//
//  Created by MNN on 2019/09/17.
//  Copyright © 2018, Alibaba Group Holding Limited
//

#include <random>
#include <math.h>
#include "ExprCreator.hpp"
#define MNN_OPEN_TIME_TRACE
#include "AutoTime.hpp"
#include "MNNTestSuite.h"
using namespace MNN::Express;
#define WIDTH 5001
#define HEIGHT 1001
#define TIME 100
class BinarySpeedTest : public MNNTestCase {
public:
    void SubTest() {
        auto input0 = _Input({WIDTH, HEIGHT});
        auto input1 = _Input({WIDTH, HEIGHT});
        auto output = _Sub(input0, input1);
        {
            AUTOTIME;
            for (int i=0; i<TIME; ++i) {
                input0->writeMap<float>();
                input1->writeMap<float>();
                output->readMap<float>();
            }
        }
    }
    void AddTest() {
        auto input0 = _Input({WIDTH, HEIGHT});
        auto input1 = _Input({WIDTH, HEIGHT});
        auto output = _Add(input0, input1);
        {
            AUTOTIME;
            for (int i=0; i<TIME; ++i) {
                input0->writeMap<float>();
                input1->writeMap<float>();
                output->readMap<float>();
            }
        }
    }
    
    virtual bool run() {
        printf("Test Binary for %d, %d x %d\n", WIDTH, HEIGHT, TIME);
        auto input0 = _Input({WIDTH, HEIGHT}, NHWC);
        auto input1 = _Input({WIDTH, HEIGHT}, NHWC);
        auto subOutput = _Sub(input0, input1);
        auto addOutput = _Add(input0, input1);
        //Check Result
        {
            for (int i=0; i<2; ++i) {
                auto s0 = input0->writeMap<float>();
                auto s1 = input1->writeMap<float>();
                for (int y=0; y<HEIGHT; ++y) {
                    for (int x=0; x<WIDTH; ++x) {
                        float r0 = ((x*x+y*y*y) % 10000) / 10000.0f;
                        float r1 = ((x+y) % 10000) / 10000.0f;
                        s0[y*WIDTH + x] = r0;
                        s1[y*WIDTH + x] = r1;
                    }
                }
                auto subPtr = subOutput->readMap<float>();
                auto addPtr = addOutput->readMap<float>();
                for (int y=0; y<HEIGHT; ++y) {
                    for (int x=0; x<WIDTH; ++x) {
                        auto destSub = s0[y*WIDTH+x] - s1[y*WIDTH+x];
                        auto destAdd = s0[y*WIDTH+x] + s1[y*WIDTH+x];
                        auto absSub = fabsf(subPtr[y*WIDTH+x] - destSub);
                        auto absAdd = fabsf(addPtr[y*WIDTH+x] - destAdd);
                        if (absSub > 1e-6 || absAdd > 1e-6) {
                            return false;
                        }
                    }
                }
            }
        }
        SubTest();
        AddTest();
        return true;
    }
};
MNNTestSuiteRegister(BinarySpeedTest, "speed/Binary");