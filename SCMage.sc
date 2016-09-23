SCMage : UGen {
    *ar { |freq = 440, timeScale = 1.0|
        ^this.multiNew('audio', freq, timeScale);
    }
}