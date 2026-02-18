onload = (event) => {
    let letterP = document.querySelector(".letter-p");
    let letterI = document.querySelector(".letter-i");
    let letterE = document.querySelector(".letter-e");
    let letterR = document.querySelector(".letter-r");

    let dot1 = document.querySelector(".dot-1");
    let dot2 = document.querySelector(".dot-2");
    let dot3 = document.querySelector(".dot-3");

    let continueText = document.querySelector(".continue-text");
    
    if (!sessionStorage.getItem("refreshCount")) {
        letterP.classList.add("p-transition");
        letterI.classList.add("i-transition");
        letterE.classList.add("e-transition");
        letterR.classList.add("r-transition");
        dot1.classList.add("dot-transition");
        dot2.classList.add("dot-transition");
        dot3.classList.add("dot-transition");
        continueText.classList.add("continue-text-with-animation");
        sessionStorage.setItem("refreshCount", 1);
    }
    else {
        letterP.classList.remove("letter-p");
        letterI.classList.remove("letter-i");
        letterE.classList.remove("letter-e");
        letterR.classList.remove("letter-r");
        dot1.classList.remove("dot-1");
        dot2.classList.remove("dot-2");
        dot3.classList.remove("dot-3");
    }
}