#ifndef E4429DDA_2009_4B80_B027_2D0590147712
#define E4429DDA_2009_4B80_B027_2D0590147712

#define MANUAL 0
#define AUTOMATIC 1
 
#define DIRECT 0
#define REVERSE 1

class PID{

    public:
        PID(void);
        double Compute(double in);

        void SetSetpoint(double sp);
        
        void SetTunings(double Kp, double Ki, double Kd);
        void GetTunings(double* Kp, double* Ki, double* Kd);
        
        void SetSampleTime(int NewSampleTime);
        
        void SetOutputLimits(double Min, double Max);
        
        void SetMode(int Mode);
        
        void Initialize();
        
        void SetControllerDirection(int Direction);

        void EnableAutoTune(double autoTuneStep);

        void DisableAutoTune();

        void AutoTuneStep();

        void Reset();

    private:
        /*working variables*/
        unsigned long lastTime = 0;
        double Input, Output, Setpoint = 0.0;
        double ITerm, lastInput = 0.0;
        double kp, ki, kd = 0.0;
        int SampleTime = 1000; // 1 sec
        double outMin, outMax;
        bool inAuto = false;

        int controllerDirection = DIRECT;
        double autoTuneStep_;       // Auto-tuning step size
        int autoTuneState_ = 0;     // Current auto-tuning state
        double autoTuneKu_ = 0.0;   // Ultimate gain during auto-tuning
        double autoTuneTu_ = 10000.0;  // Ultimate period during auto-tuning (default value: 10 seconds)
        double autoTuneKc_ = 0.0;   // Ultimate gain for PID tuning
        double autoTuneTi_ = 0.0;   // Ultimate period for integral time constant
        double autoTuneTd_ = 0.0;   // Ultimate period for derivative time constant
        double autoTuneKp_ = 0.0;   // Tuned proportional gain
        double autoTuneKi_ = 0.0;   // Tuned integral gain
        double autoTuneKd_ = 0.0;   // Tuned derivative gain
        double autoTuneOutput_ = 0.0; // Output value during auto-tuning
        double autoTuneInitialOutput_ = 0.0; // Initial output value for auto-tuning
        double autoTuneSetpoint_ = 0.0; // Setpoint for auto-tuning
        double autoTuneStartTime_ = 0; // Start time of auto-tuning (iteration count)
};


#endif /* E4429DDA_2009_4B80_B027_2D0590147712 */
