/**********************************************************
 * University of Warsaw, Department of Biomedical Physics *
 *   Enhanced Matching Pursuit Implementation (empi)      *
 * See README.md and LICENCE for details.                 *
 **********************************************************/
#include <limits>
#include "io.hpp"

MultiSignal SignalReader::read() const {
	MultiSignal result;
	std::vector<float> sample(channelCount);
	const int C = selectedChannels.size();
	result.channels.resize(C);
	for (int i=0; i<C; ++i) {
		result.channels[i].freqSampling = freqSampling;
	}
	FILE* file = fopen(pathToSignalFile.c_str(), "rb");
	if (!file) {
		throw Exception("couldNotOpenSignalFile");
	}
	int leftToStore = std::numeric_limits<int>::max();
	while (fread(sample.data(), sizeof(float), channelCount, file) == static_cast<size_t>(channelCount)) {
		if (--leftToStore < 0) {
			throw Exception("signalFileIsTooLongForThisMachine");
		}
		for (int i=0; i<C; ++i) {
			result.channels[i].samples.push_back(sample[selectedChannels[i]-1]);
		}
	}
	fclose(file);
	return result;
}

void BookWriter::write(const MultiSignal& signal, const MultiChannelResult& result) const {
	BookHeader headerStart;
	BookDataHeader headerData;
	BookDataSignalHeader headerSignal;
	BookDataAtomsHeader headerAtoms;
	BookDataAtomHeader headerAtom;

	FILE* file = fopen(pathToBookFile.c_str(), "wb");
	if (!file) {
		throw Exception("couldNotCreateOutputFile");
	}

	const int C = signal.channels.size();
	const int N = C ? signal.channels.front().samples.size() : 0;

	setBE(headerStart.channelCount, C);
	setBE(headerStart.dictionarySize, 0);
	setBE(headerStart.energyPercent, 100.0);
	setBE(headerStart.iterationCount, 1);
	setBE(headerStart.pointsPerMicrovolt, 1.0);
	setBE(headerStart.freqSampling, C ? signal.channels.front().freqSampling : 0.0);
	fwrite(&headerStart, sizeof headerStart, 1, file);

	long headerDataPosition = ftell(file);
	setBE(headerData.epochNumber, 1);
	setBE(headerData.sampleCount, N);
	fwrite(&headerData, sizeof headerData, 1, file);

	std::vector<float> sampleBuffer(N);
	std::vector<float> paramsBuffer;
	for (int c=0; c<C; ++c) {
		setBE(headerSignal.channelNumber, c+1);
		setBE(headerSignal.len, sizeof headerSignal.channelNumber + sizeof(float) * N);
		fwrite(&headerSignal, sizeof headerSignal, 1, file);
		for (int i=0; i<N; ++i) {
			setBE(sampleBuffer[i], signal.channels[c].samples[i]);
		}
		fwrite(sampleBuffer.data(), sizeof(float), N, file);

		long headerAtomsPosition = ftell(file);
		setBE(headerAtoms.channelNumber, c+1);
		fwrite(&headerAtoms, sizeof headerAtoms, 1, file);
		for (const Atom& atom : result[c]) {
			const size_t P = atom.params.size();
			setBE(headerAtom.len, sizeof(float) * P);
			setBE(headerAtom.type, atom.type);
			fwrite(&headerAtom, sizeof headerAtom, 1, file);
			paramsBuffer.resize(atom.params.size());
			for (size_t p=0; p<P; ++p) {
				setBE(paramsBuffer[p], atom.params[p]);
			}
			fwrite(paramsBuffer.data(), sizeof(float), P, file);
		}

		long currentPosition = ftell(file);
		fseek(file, headerAtomsPosition, SEEK_SET);
		setBE(headerAtoms.len, currentPosition - headerAtomsPosition - 5);
		fwrite(&headerAtoms, sizeof headerAtoms, 1, file);
		fseek(file, currentPosition, SEEK_SET);
	}

	long currentPosition = ftell(file);
	fseek(file, headerDataPosition, SEEK_SET);
	setBE(headerData.len, currentPosition - headerDataPosition - 5);
	fwrite(&headerData, sizeof headerData, 1, file);
	fseek(file, currentPosition, SEEK_SET);

	fclose(file);
}
