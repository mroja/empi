/**********************************************************
 * University of Warsaw, Department of Biomedical Physics *
 *   Enhanced Matching Pursuit Implementation (empi)      *
 * See README.md and LICENCE for details.                 *
 **********************************************************/
#ifndef EMPI_CLASSES_HPP
#define	EMPI_CLASSES_HPP

#include "base.hpp"

template<typename T>
class TimeFreqMap {
	T* pointer = nullptr;
	double* fValues = 0;
	double* tValues = 0;

	TimeFreqMap(const TimeFreqMap&) =delete;
	void operator=(const TimeFreqMap&) =delete;

public:	
	const int cCount, fCount, tCount;

	TimeFreqMap(int cCount, int fCount, int tCount)
	: cCount(cCount), fCount(fCount), tCount(tCount) {
		pointer = static_cast<T*>(calloc(fCount * tCount * cCount, sizeof(T)));
		fValues = static_cast<double*>(calloc(fCount, sizeof(double)));
		tValues = static_cast<double*>(calloc(tCount, sizeof(double)));
	}

	~TimeFreqMap() {
		free(pointer);
		free(fValues);
		free(tValues);
	}

	inline double& f(int fIndex) {
		return fValues[fIndex];
	}

	inline const double& f(int fIndex) const {
		return fValues[fIndex];
	}

	inline double& t(int tIndex) {
		return tValues[tIndex];
	}

	inline const double& t(int tIndex) const {
		return tValues[tIndex];
	}

	inline T& value(int cIndex, int fIndex, int tIndex) {
		return pointer[(cIndex * fCount + fIndex) * tCount + tIndex];
	}

	inline const T& value(int cIndex, int fIndex, int tIndex) const {
		return pointer[(cIndex * fCount + fIndex) * tCount + tIndex];
	}
};

//------------------------------------------------------------------------------

typedef void (*MultichannelConstraint)(std::vector<complex>&);

class Workspace {
public:
	static void subtractAtomFromSignal(Atom& atom, SingleSignal& signal, bool fit);

	virtual ~Workspace() =default;
	virtual Atoms findBestMatch(MultichannelConstraint constraint = nullptr) const =0;
	virtual size_t getAtomCount(void) const =0;
	virtual void subtractAtom(const Atom& atom, SingleSignal& signal, int channel) =0;
};

class WorkspaceBuilder {
public:
	virtual ~WorkspaceBuilder() =default;
	virtual Workspace* buildWorkspace(const MultiSignal&) const =0;
};

//------------------------------------------------------------------------------

struct DecompositionSettings {
	int iterationMax;
	double residualEnergy;
};

class Decomposition {
	const MultichannelConstraint constraint;

protected:
	Decomposition(MultichannelConstraint constraint)
	: constraint(constraint) { }

public:
	virtual ~Decomposition() =default;
	virtual MultiChannelResult compute(const DecompositionSettings& settings, const WorkspaceBuilder& builder, const MultiSignal& signal);
};

//------------------------------------------------------------------------------

class SmpDecomposition : public Decomposition {
public:
	SmpDecomposition(void) : Decomposition(nullptr) { }

	MultiChannelResult compute(const DecompositionSettings& settings, const WorkspaceBuilder& builder, const MultiSignal& signal);
};

#endif /* EMPI_CLASSES_HPP */
