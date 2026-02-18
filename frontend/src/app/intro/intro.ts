import { Component, OnInit } from '@angular/core';

@Component({
  selector: 'intro',
  imports: [],
  templateUrl: './intro.html',
  styleUrl: './intro.less',
})
export class Intro implements OnInit {

  firstLoad = true;

  constructor(){}

  ngOnInit(): void {
    if (!sessionStorage.getItem("refreshCount")) {
      // this.letterP.classList.add("p-transition");
      // this.letterI.classList.add("i-transition");
      // this.letterE.classList.add("e-transition");
      // this.letterR.classList.add("r-transition");
      // this.dot1.classList.add("dot-transition");
      // this.dot2.classList.add("dot-transition");
      // this.dot3.classList.add("dot-transition");
      // this.continueText.classList.add("continue-text-with-animation");

      this.firstLoad = true;
      sessionStorage.setItem("refreshCount", "1");
    }
    else {
      // this.letterP.classList.remove("letter-p");
      // this.letterI.classList.remove("letter-i");
      // this.letterE.classList.remove("letter-e");
      // this.letterR.classList.remove("letter-r");
      // this.dot1.classList.remove("dot-1");
      // this.dot2.classList.remove("dot-2");
      // this.dot3.classList.remove("dot-3");
    }
  }
}
