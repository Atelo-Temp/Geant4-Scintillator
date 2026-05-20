#ifndef MyTimer_HH
#define MyTimer_HH

// ...
class Timer {
    public:
        // Constructor
        Timer() = default;
        
        // Destructor
        ~Timer() = default;
        
        // To be called by StartTimer for runtime tracking, or externally for random seeding
        long GetTimestamp();
        
        // To be called at the start of a run, for runtime tracking
        long StartTimer();
        
        // To be called at the end of a run, for runtime calculation
        void EndTimer();
    private:
        // Stores millisecond timestamp corresponding to start of the run
        long fStartTime = -1;
};

#endif
