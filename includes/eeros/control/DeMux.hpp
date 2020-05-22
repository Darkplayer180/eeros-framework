#ifndef ORG_EEROS_CONTROL_DEMUX_HPP_
#define ORG_EEROS_CONTROL_DEMUX_HPP_

#include <eeros/control/Block1i.hpp>
#include <eeros/math/Matrix.hpp>
#include <eeros/control/Output.hpp>

namespace eeros {
	namespace control {
		
		template < uint32_t N, typename T = double, typename C = eeros::math::Matrix<N,1,T> >
		class DeMux: public Block1i<C> {
		public:
			DeMux() { 
				for(uint32_t i = 0; i < N; i++) {
					this->out[i].getSignal().clear();
					out[i].setOwner(this);
				}
			}
			
			virtual void run() {
				for(uint32_t i = 0; i < N; i++) {
					out[i].getSignal().setValue(this->in.getSignal().getValue()(i));
					out[i].getSignal().setTimestamp(this->in.getSignal().getTimestamp());
				}
			}
			
			virtual Output<T>& getOut(uint32_t index) {
				return out[index];
			}
			
		protected:
			Output<T> out[N];
		};

	};
};

#endif /* ORG_EEROS_CONTROL_DEMUX_HPP_ */
