import { AfterViewInit, Component, Output, EventEmitter, HostListener, OnInit, Input, OnDestroy } from '@angular/core';
import { CommonModule } from '@angular/common';
import { AnimationDataEmitter } from '../../services/animation-data-emitter';
import { from, Subscription } from 'rxjs';
import { Router } from '@angular/router';
import { ComponentServ } from '../../services/component';
import { animatedLetter, animationData, continuteTextOptions } from '../../models/animation';

@Component({
  selector: 'intro',
  imports: [CommonModule],
  templateUrl: './intro.html',
  styleUrl: './intro.less',
})
export class Intro implements OnInit, AfterViewInit, OnDestroy {

  @Output() changed = new EventEmitter<boolean>()

  private toolbarSubscription: Subscription = new Subscription();
  private animationChangeSubscription: Subscription = new Subscription();
  private refreshFuncSubscription: Subscription = new Subscription();

  firstLoad = false;
  animationEnd = false;
  animationTextList: animatedLetter[] = [];
  recievedData: animationData = { mainText: "", continuteText: "", animationDuration: 0, animationDelay: 0 };
  continueText: string = "";
  continuteTextAnimationDelay: number = 0;

  constructor(
    private dataService: AnimationDataEmitter,
    private router: Router,
    private componentService: ComponentServ
  ){}

  @HostListener('document:click', ['$event'])
  handleClick(event: MouseEvent) {
    if (this.animationEnd)
      this.changed.emit(true);   
  }

  ngOnInit(): void {
    this.animationChangeSubscription = this.dataService.currentData.subscribe(data => {
      this.recievedData = data;
    });

    this.toolbarSubscription = this.dataService.triggerAction$.subscribe(data => {
      this.updateAnimation();
    });

    this.refreshFuncSubscription = this.componentService.triggerRefresh$.subscribe(() => {
      this.refreshViewport();
    });

    if (this.recievedData) { this.updateAnimation(); }
  }

  ngAfterViewInit(): void {
    requestAnimationFrame(() => {
        this.firstLoad = true;
    });
  }

  updateAnimation() {
    let _transitionDelay = 1;
    let _transitioning = "";
    let _fromAmount = "";
    let _transitionDuration = 1.5;
    this.animationTextList = [];
    
    if (this.recievedData.animationDuration != 0) {
      _transitionDuration = this.recievedData.animationDuration;
    }

    if (this.recievedData.animationDelay != 0) {
      _transitionDelay = this.recievedData.animationDelay;
    }

    if (this.recievedData.mainText != "") {
      const letterArray = this.recievedData.mainText.split('');
      letterArray.forEach((_letter, index) => {
        
        if (index == 0) { _transitioning = "left"; _fromAmount = "-60vw"; }
        else if (index == letterArray.length - 1) { _transitioning = "right"; _fromAmount = "-60vw"; }
        else if (index % 2 == 0) { _transitioning = "top"; _fromAmount = "-60vh"; }
        else if (index % 2 != 0) { _transitioning = "bottom"; _fromAmount = "-60vh"; }

        this.animationTextList.push({letter: _letter, transitioningFrom: _transitioning, amountToFrom: _fromAmount, transitionTiming: `${_transitionDuration}s`, transitionDelay: `${_transitionDelay++}s`})
      });
    }

    if (this.recievedData.continuteText != "") {
      this.continueText = this.recievedData.continuteText;
    }

    this.continuteTextAnimationDelay = _transitionDelay;
  }

  letterStyles(animationLetter: any) {
    return {
      [animationLetter.transitioningFrom]: animationLetter.amountToFrom,
      transition: `${animationLetter.transitioningFrom} ${animationLetter.transitionTiming}`,
      'transition-delay': animationLetter.transitionDelay
    };
  }

  getTransition(transitioningFrom: string) {
    return {
      [`${transitioningFrom}-transition`]: this.firstLoad
    };
  }

  continuteTextStyle() {
    return {
      '-webkit-animation-delay': this.continuteTextAnimationDelay + 's',
      '-moz-animation-delay': this.continuteTextAnimationDelay + 's',
      'animation-delay': this.continuteTextAnimationDelay + 's',
    };
  }

  onAnimationEnd() {
    this.animationEnd = true;
  }

  refreshViewport() {
    const currentUrl = this.router.url;
    this.router.navigateByUrl('/refresh', { skipLocationChange: true }).then(() => {
      this.router.navigate([currentUrl]);
    });
  }

  ngOnDestroy(): void {
    this.toolbarSubscription.unsubscribe();
    this.animationChangeSubscription.unsubscribe();
    this.refreshFuncSubscription.unsubscribe();
  }
}
