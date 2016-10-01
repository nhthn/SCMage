SCMage : UGen {
    *ar { |bufnum, freqValue = 0.0, freqMode = -1, timeScale = 1.0, alpha = 0.55|
        ^this.multiNew('audio', bufnum, freqValue, freqMode, timeScale, alpha);
    }
}