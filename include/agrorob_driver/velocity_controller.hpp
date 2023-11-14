#pragma once
#include <iostream>
#include <agrorob_driver/digital_filters.hpp>
// #include <agrorob_driver/adrc_eso.hpp>


namespace agrorob_interface
{
    class VelocityController
    {

    double throttle_;
    double brake_;
    
    double dt_ = 0.0; 
    double controlScale = 100.0;
    //BCM pin numbering mode
    // double kd = 0.4; 
    // double ki = 0.1;
    double kd = 0.15; 
    double ki = 0.03;


    double kf = 0.0;

    double errorIntegral = 0.0;

    double prevVelocity = 0.0;
    LowPassFilter3 velocityRateFilter;
    
    double prevControl = 0.0;

    int direction = 0; //0 - the robot is not moving; 1 - forward drive  2 - driving backwards 
    int ThrottleValue = 0;

    int breakCounter = 0;
    

    

    public:

        VelocityController(double loopRate) 
        : dt_(1.0/loopRate) , velocityRateFilter(dt_ , 200.0)
        {
            
        }


        double getDirection()
        {
            return direction;
            // return 0;

        }
        
        
        
        double getThrottle()
        {
            return ThrottleValue;
        //    return 0.0;
        }
        
      

        void update(double velocity, double referenceVelocity, double referenceVelocityRate )
        {
            
      

           
            // If going backwards, change sign of refVel to opposite, control then is the same as going forward
            // if (tbVel_->getGear() == 2){
            //     referenceVelocity = -referenceVelocity;
            // }
            
            double velocityError = referenceVelocity - velocity;
            double controlSignal = 0.0;

            // if (velocity <= referenceVelocity && referenceVelocity > 0.0)
            //     ThrottleValue = 0;
            

            if (velocity > (1.5*abs(referenceVelocity)) ) {
                ThrottleValue = 0;
                direction = 0;
                // controlSignal = -1.0;

            } else {

                if (referenceVelocity <  - 0.01)
                    direction = 2;

                else if (referenceVelocity >  0.01)
                    direction = 1;

                else
                    direction = 0; 


                errorIntegral += velocityError*(dt_);
                double velocityRate = velocityRateFilter.update((velocity - prevVelocity)*dt_);
                prevVelocity = velocity;
                double velocityRateError = referenceVelocityRate - velocityRate;
                

                controlSignal = kd*velocityError + ki*errorIntegral + kf*velocityRateError;



                if (controlSignal > 1.0)
                    controlSignal = 1.0;
                if (controlSignal < -1.0)
                    controlSignal = -1.0;

                controlSignal = abs(controlSignal);
                
            }

            // RCLCPP_INFO_STREAM(nh_->get_logger(), "Control signal: " << controlSignal);

            ThrottleValue = controlSignal*controlScale;

            if ((referenceVelocity > 0.0 && direction == 2) || (referenceVelocity < 0.0 && direction == 1))
            {
                if (velocity > 0.02)
                    ThrottleValue = 0;   //else is to change direction of move later
        
            }


        }
    };
}