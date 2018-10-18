#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <chrono>
#include <vector>
#include <stdio.h>

/****************************************************************************/
/***                                                                      ***/
/***   Basic Benchmark Macros for performance tests                       ***/
/***                                                                      ***/
/****************************************************************************/

#ifndef GP_NO_BENCHMARK
    struct BENCHMARK_TEST
    {
        BENCHMARK_TEST()
        {
            BENCHMARK_LOOPS = 1;
            BENCHMARK_FUNC_NAME = "NO NAME";
            BENCHMARK_IS_BLOCK = true;
            BENCHMARK_IS_FINISHED = false;
        }

        bool BENCHMARK_IS_BLOCK;
        unsigned int BENCHMARK_LOOPS;
        std::chrono::high_resolution_clock::time_point BENCHMARK_TIME_START;
        std::chrono::high_resolution_clock::time_point BENCHMARK_TIME_END;
        const char* BENCHMARK_FUNC_NAME;
        bool BENCHMARK_IS_FINISHED;
    };

    static std::vector<BENCHMARK_TEST*> BENCHMARK_ARRAY;

    //---------------------------------------------------------------------------------------------
    // benchmark a function with one or multiple iterations
    //---------------------------------------------------------------------------------------------
    #define GP_BENCHMARK_FUNC(func, nb_loop) \
    { \
        BENCHMARK_TEST * bench = new BENCHMARK_TEST(); \
        bench->BENCHMARK_LOOPS = nb_loop; \
        bench->BENCHMARK_IS_BLOCK = false; \
        bench->BENCHMARK_FUNC_NAME = #func; \
        bench->BENCHMARK_TIME_START = std::chrono::high_resolution_clock::now(); \
        for (unsigned int i=0; i<nb_loop; ++i) func; \
        bench->BENCHMARK_TIME_END = std::chrono::high_resolution_clock::now(); \
        bench->BENCHMARK_IS_FINISHED = true; \
        BENCHMARK_ARRAY.push_back(bench); \
    }

    //---------------------------------------------------------------------------------------------
    // benchmark block begin, put code to test between block begin/end macros
    //---------------------------------------------------------------------------------------------
    #define GP_BENCHMARK_BLOCK_BEGIN(blockName) \
    { \
        BENCHMARK_ARRAY.push_back(new BENCHMARK_TEST); \
        BENCHMARK_ARRAY.back()->BENCHMARK_FUNC_NAME = blockName; \
        BENCHMARK_ARRAY.back()->BENCHMARK_IS_FINISHED = false; \
        BENCHMARK_ARRAY.back()->BENCHMARK_TIME_START = std::chrono::high_resolution_clock::now(); \
    }

    //---------------------------------------------------------------------------------------------
    // benchmark block end
    //---------------------------------------------------------------------------------------------
    #define GP_BENCHMARK_BLOCK_END() \
    { \
        auto end = std::chrono::high_resolution_clock::now(); \
        if(BENCHMARK_ARRAY.size() > 0 && BENCHMARK_ARRAY.back()->BENCHMARK_IS_BLOCK && \
           BENCHMARK_ARRAY.back()->BENCHMARK_IS_FINISHED == false) { \
        BENCHMARK_ARRAY.back()->BENCHMARK_TIME_END = end; \
        BENCHMARK_ARRAY.back()->BENCHMARK_IS_FINISHED = true; \
        } \
        else printf("WARNING: BENCHMARK_BLOCK_END (missing BENCHMARK_BLOCK_BEGIN ?) !!!\n");\
    }

    //---------------------------------------------------------------------------------------------
    // report benchmark measures
    //---------------------------------------------------------------------------------------------
    #define GP_BENCHMARK_REPORT() \
    { \
        printf("=================== BENCHMARK REPORT ==================================\n"); \
        printf("num\tseconds\t\tloop\tdescription\n"); \
        for (size_t i=0; i<BENCHMARK_ARRAY.size(); ++i) \
        { \
            if(!BENCHMARK_ARRAY[i]->BENCHMARK_IS_FINISHED && BENCHMARK_ARRAY[i]->BENCHMARK_IS_BLOCK) {\
                printf("%d: WARNING: MISSING nBENCHMARK_BLOCK_END!!!\n", i); \
                break; \
            }\
            else { \
                auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(BENCHMARK_ARRAY[i]->BENCHMARK_TIME_END - BENCHMARK_ARRAY[i]->BENCHMARK_TIME_START); \
                printf("%d\t[%lf s]\t%8d\t%s\n" \
                    ,i \
                    ,time_span.count() \
                    ,BENCHMARK_ARRAY[i]->BENCHMARK_LOOPS \
                    ,BENCHMARK_ARRAY[i]->BENCHMARK_FUNC_NAME \
                    ); \
            } \
        } \
        printf("=======================================================================\n"); \
        BENCHMARK_ARRAY.clear(); \
    }

#else
    #define GP_BENCHMARK_FUNC(f,l) f
    #define GP_BENCHMARK_BLOCK_BEGIN(n)
    #define GP_BENCHMARK_BLOCK_END()
    #define GP_BENCHMARK_REPORT()
#endif


#endif // BENCHMARK_H
