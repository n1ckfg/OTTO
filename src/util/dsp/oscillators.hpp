#include <Gamma/gen.h>
#include <Gamma/scl.h>
#include <Gamma/tbl.h>
#include <Gamma/Strategy.h>
#include <Gamma/Domain.h>
#include <Gamma/Types.h>
#include <Gamma/Oscillator.h>

/// Periodic waveforms to be used as sound sources


template<class Tv = gam::real, class Td = gam::DomainObserver>
class DoubleBuzz : public gam::AccumPhase<Tv,Td> {
public:

	/// \param[in]	frq			Frequency
	/// \param[in]	phase		Phase in [0, 1)
	/// \param[in]	harmonics	Number of harmonics
	DoubleBuzz(Tv frq=440, Tv phase=0, Tv pw=M_PI_2, Tv morph=0.5, Tv harmonics=8);
	virtual ~DoubleBuzz(){}

	void antialias();			///< Adjust number of harmonics to prevent aliasing
	void harmonics(Tv num);		///< Set number of harmonics
	void harmonicsMax();		///< Set number of harmonics to fill Nyquist range
	void normalize(bool v);		///< Whether to normalize amplitude

	Tv operator()();			///< Returns next sample of all harmonic impulse
	Tv pulse(Tv intg=0.999);	///< Returns next sample of square waveform
	
	Tv maxHarmonics() const;	///< Get number of harmonics below Nyquist based on current settings

    Tv morph() const;           /// Get morph value
    void morph(Tv val);         /// Set morph value

    Tv pulsewidth() const;      /// Gets pulsewidth
    void pulsewidth(Tv val);    /// Sets pulsewidth

	void onDomainChange(double r);

protected:
	Tv mAmp;			// amplitude normalization factor
	Tv mN;				// actual number of harmonics
	Tv mNDesired;		// desired number of harmonics
	Tv mNFrac;		
	Tv mSPU_2;			// cached local
	Tv mPrev;			// previous output for integration
    Tv w_;              // Offset to second phase. Goes from 0 to pi. Equivalent to pulsewidth of the square
    Tv s_;              // Scaling of the second BLIT. Acts as a morph parameter between saw and square
	bool mNormalize;
	void setAmp();
private: typedef gam::AccumPhase<Tv,Td> Base;
};

/// Band-limited impulse train with adjustable offset to phaseacc.
template <class Tv = gam::real, class Td = gam::DomainObserver>
struct DoubleBLIT : public DoubleBuzz<Tv,Td>{
public:

	/// \param[in] frq		Frequency
	/// \param[in] phs		Phase, in [0, 1)
	DoubleBLIT(Tv frq=440, Tv phs=0, Tv pw=M_PI_2, Tv morph=0.5): Base(frq, phs, pw, morph){ onDomainChange(1); }

	/// Set frequency
	void freq(Tv v){ Base::freq(v); Base::harmonicsMax(); }

	void onDomainChange(double r){
		Base::onDomainChange(r);
		freq(gam::AccumPhase<Tv,Td>::freq());
	}

	using DoubleBuzz<Tv,Td>::freq; // needed for getter

private: typedef DoubleBuzz<Tv,Td> Base;
};

/// Band-limited saw and pulse wave

/// This produces a Fourier representation of a saw wave where the number of
/// harmonics is adjusted automatically to prevent aliasing.
/// Due to numerical issues, this generator should not be used for producing 
/// very low frequency modulation signals. For that purpose, it is better to use
/// the LFO class.
///
/// \tparam Tv	Value (sample) type
/// \tparam Td	Domain type
/// \ingroup Oscillator 
template <class Tv = gam::real, class Td = gam::DomainObserver>
struct MultiOsc : public DoubleBLIT<Tv,Td> {

	/// \param[in] frq		Frequency
	/// \param[in] phs		Phase, in [0, 1)
	MultiOsc(Tv frq=440, Tv phs=0, Tv pw=M_PI_2, Tv morph=0.5): DoubleBLIT<Tv, Td>(frq, phs, pw, morph){}

	/// Generate next sample
	
	/// \param[in] itg		Leaky integration factor
	///
    /// Get next sample in the pulse wave
    Tv pulse(Tv itg=0.999){ 
        return current_pulse_sample = DoubleBLIT<Tv,Td>::pulse(itg); 
    }
	/// Get Saw/Tri sample. Note that this one is an integration of the pulse wave
    /// and does not increment the phase
    Tv integrated_quick(Tv itg=0.98){ return last_sawtri_sample = current_pulse_sample + itg * last_sawtri_sample; }
private:
    Tv current_pulse_sample = 0;
    Tv last_sawtri_sample = 0;
};

// Implementation_______________________________________________________________

//---- DoubleBuzz
template<class Tv, class Td> DoubleBuzz<Tv,Td>::DoubleBuzz(Tv f, Tv p, Tv pw, Tv morph, Tv harmonics)
:	Base(f, p), mAmp(0), mPrev(Tv(0)), w_(pw), s_(morph), mNormalize(true)
{
	onDomainChange(1);
	this->harmonics(harmonics);
}

template<class Tv, class Td> inline void DoubleBuzz<Tv,Td>::harmonics(Tv v){
	mNDesired = v;
	mN = gam::scl::floor(v);
	mNFrac = v - mN;
	setAmp();
}

template<class Tv, class Td> inline void DoubleBuzz<Tv,Td>::harmonicsMax(){
	harmonics(maxHarmonics());
}

template<class Tv, class Td> inline void DoubleBuzz<Tv,Td>::antialias(){
	Tv newN = maxHarmonics();
	newN = mNDesired > newN ? newN : mNDesired;

	mN = gam::scl::floor(newN);
	mNFrac = newN - mN;
	setAmp();
}

template<class Tv, class Td> void DoubleBuzz<Tv,Td>::normalize(bool v){
	mNormalize = v;
	setAmp();
}

template<class Tv, class Td> inline Tv DoubleBuzz<Tv,Td>::maxHarmonics() const {
	return mSPU_2 / this->freq();
}

template<class Tv, class Td> inline void DoubleBuzz<Tv,Td>::setAmp(){

	if(mNormalize){
		// Normally, the amplitude is 1/(2N), but we will linearly interpolate
		// based on fractional harmonics to avoid sudden changes in amplitude to
		// the lower harmonics which is very noticeable.
		mAmp = (mN != Tv(0)) ? (Tv(0.5) / (mN+mNFrac)) : 0;
	}
	else{
		mAmp = Tv(0.5);
	}
}

template<class Tv, class Td> 
inline Tv DoubleBuzz<Tv,Td>::morph() const { return s_; }

template<class Tv, class Td> 
inline void DoubleBuzz<Tv,Td>::morph(Tv val) { s_ = val; }

template<class Tv, class Td> 
inline Tv DoubleBuzz<Tv,Td>::pulsewidth() const { return w_; }

template<class Tv, class Td> 
inline void DoubleBuzz<Tv,Td>::pulsewidth(Tv val) { w_ = gam::scl::wrap(val, Tv(M_PI), Tv(0)); }

#define EPS 0.000001
template<class Tv, class Td> inline Tv DoubleBuzz<Tv, Td>::operator()(){
	/*        1   / sin((N+0.5)x)    \
	   f(x) = -- |  ------------- - 1 |
	          2N  \   sin(0.5x)      /
	*/
	Tv theta = this->nextPhase();
    Tv theta2 = gam::scl::wrapPhaseOnce(theta + w_);

	Tv result;

    // Calculate first BLIT
	Tv denom = gam::scl::sinT7(theta * Tv(0.5));
	// denominator goes to zero when theta is an integer multiple of 2 pi
	if(gam::scl::abs(denom) < Tv(EPS)){
		result = Tv(2) * mN * mAmp;
		//printf("Buzz::operator(): oops\n");
	}
	else{
		Tv nphase = gam::scl::wrapPhase(theta * (mN + Tv(0.5)));
		//result = (scl::sinT7(nphase) / denom - Tv(1)) * mAmp;
		result = ((gam::scl::sinT7(nphase) - denom) / denom) * mAmp;
	}

    // Calculate second BLIT
	denom = gam::scl::sinT7(theta2 * Tv(0.5));
	// denominator goes to zero when theta is an integer multiple of 2 pi
	if(gam::scl::abs(denom) < Tv(EPS)){
		result -= Tv(2) * mN * mAmp * s_;
		//printf("Buzz::operator(): oops\n");
	}
	else{
		Tv nphase = gam::scl::wrapPhase(theta2 * (mN + Tv(0.5)));
		//result = (scl::sinT7(nphase) / denom - Tv(1)) * mAmp;
		result -= ((gam::scl::sinT7(nphase) - denom) / denom) * mAmp * s_;
	}

	return result;
}
#undef EPS

template<class Tv, class Td>
inline Tv DoubleBuzz<Tv,Td>::pulse(Tv b){ return mPrev=(*this)() + b*mPrev; }

template<class Tv, class Td> void DoubleBuzz<Tv,Td>::onDomainChange(double r){
	Base::onDomainChange(r);
	mSPU_2 = Tv(Td::spu() * 0.5);
}