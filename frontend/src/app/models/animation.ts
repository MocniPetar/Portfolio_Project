export interface animatedLetter {
    letter: string,
    transitioningFrom: string,
    amountToFrom: string,
    transitionTiming: string,
    transitionDelay: string
}

export interface animatedLetters {
    object: animatedLetter;
}

export interface continuteTextOptions {
    text: string,
    animationDelay: string
}

export interface animationData {
    mainText: string,
    continuteText: string,
    animationDuration: number,
    animationDelay: number
}