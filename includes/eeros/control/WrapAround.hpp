#ifndef ORG_EEROS_CONTROL_WRAPAROUND_HPP_
#define ORG_EEROS_CONTROL_WRAPAROUND_HPP_

#include <eeros/control/Blockio.hpp>
#include <eeros/math/Matrix.hpp>


namespace eeros {
namespace control {
/**
* The WrapAround block wraps an input value between two limit values. 
* The output value will also vary between minVal and maxVal.
* 
* @tparam Tout - output type (double - default type) 
* @tparam Twrap - type of min and max values for wrap. Must be double, or same as Tout (double - default type) 
* @tparam minVal: minimum value - lower value of output signal
* @tparam maxVal: maximum value - higher value of output signal
*  
* @since xxx
*/
	
	template<typename Tout = double, typename Twrap = double>
	class WrapAround : public eeros::control::Blockio<1,1,Tout>
	{
		public:
			/**
			 * Constructs a WrapAround instance specifying minValue and maxValue of output 
			 * to realize wrap. \n
			 */
			WrapAround(Twrap minVal, Twrap maxVal) : enabled(true) {
				this->minVal = minVal;
				this->maxVal = maxVal;
			}
			
			/**
			 * Disabling use of copy constructor because the block should never be copied unintentionally.
			 */
			WrapAround(const WrapAround& s) = delete; 
	
			/**
			 * Runs the Wrap Around Algrithm, as described above.
			 * 
			 */
			virtual void run(){
				std::lock_guard<std::mutex> lock(mtx);
				Tout inVal = this->in.getSignal().getValue();
				Tout outVal = inVal;
				
				if(enabled) {
                    outVal = calculateResult<Tout>(inVal);
				}
					
				this->out.getSignal().setValue(outVal);
				this->out.getSignal().setTimestamp(this->in.getSignal().getTimestamp());
			}
			
			/**
			 * Enables the rate limiter block.
			 * 
			 * If enabled, run() will perform rate limit.
			 * 
			 * @see disable()
			 */
			virtual void enable() {
				enabled = true;
			}
			
			/**
			 * Disables the rate limiter block.
			 * 
			 * If disabled, run() will set output = input.
			 * 
			 * @see enable()
			 */
			virtual void disable() {
				enabled = false;
			}
			
			/**
			* Sets min and max values.
			*/
			virtual void setMinMax(Twrap minVal, Twrap maxVal) {
				std::lock_guard<std::mutex> lock(mtx);
				this->minVal = minVal;
				this->maxVal = maxVal;
			}
        
        private:
            template <typename S> 
            typename std::enable_if<std::is_arithmetic<S>::value, S>::type calculateResult(S inValue) {
                Tout outVal;
                double delta = fabs(this->minVal) + fabs(this->maxVal);
                    
                double num = inValue - this->minVal;
                double den = delta;
                double tquot = floor(num/den);
                outVal = num - tquot * den;
                
                if(outVal<0) {
                    outVal = outVal + delta;
                }
                outVal = outVal + this->minVal; 
                
                return outVal;
            }

            template <typename S> 
            typename std::enable_if<std::is_compound<S>::value && std::is_arithmetic<Twrap>::value, S>::type calculateResult(S inValue) {
                Tout outVal;
                for(int i=0; i<inValue.size(); i++) {
                    double delta = fabs(this->minVal) + fabs(this->maxVal);
                    
                    double num = inValue[i] - this->minVal;
                    double den = delta;
                    double tquot = floor(num/den);
                    outVal[i] = num - tquot * den;
                    
                    if(outVal[i]<0) {
                        outVal[i] = outVal[i] + delta;
                    }
                    outVal[i] = outVal[i] + this->minVal; 
                }
                return outVal;
            }
            
            template <typename S> 
            typename std::enable_if<std::is_compound<S>::value && std::is_compound<Twrap>::value, S>::type calculateResult(S inValue) {
                Tout outVal;
                for(int i=0; i<inValue.size(); i++) {
                    double delta = fabs(this->minVal[i]) + fabs(this->maxVal[i]);
                    
                    double num = inValue[i] - this->minVal[i];
                    double den = delta;
                    double tquot = floor(num/den);
                    outVal[i] = num - tquot * den;
                    
                    if(outVal[i]<0) {
                        outVal[i] = outVal[i] + delta;
                    }
                    outVal[i] = outVal[i] + this->minVal[i]; 
                }
                return outVal;
            }
            
		protected:
			bool enabled{true};
			std::mutex mtx;
			Twrap minVal{0.0};
			Twrap maxVal{0.0};
	};
}
}

#endif /* ORG_EEROS_CONTROL_WRAPAROUND_HPP_ */