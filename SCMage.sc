SCMage : UGen {
    *ar { |buf, freqValue = 0.0, freqMode = -1, timeScale = 1.0|
        ^this.multiNew('audio', buf, freqValue, freqMode, timeScale);
    }
}