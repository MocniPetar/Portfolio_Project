import { AfterViewInit, Component, ElementRef, OnDestroy, OnInit, ViewChild } from '@angular/core';
import { Router, RouterOutlet } from '@angular/router';
import { ToolbarComponent } from './components/toolbar/toolbar.component';
import { MatIconModule } from "@angular/material/icon";
import { CommonModule } from '@angular/common';
import { ComponentServ } from './services/component';

@Component({
  selector: 'app-root',
  imports: [ToolbarComponent, RouterOutlet, MatIconModule, CommonModule],
  templateUrl: './app.html',
  styleUrl: './app.less'
})
export class App implements OnInit, AfterViewInit, OnDestroy {

  @ViewChild('sideBtn') sideBtn!: ElementRef<HTMLDivElement>;
  @ViewChild('toolbar') toolbar!: ElementRef<ToolbarComponent>;

  isClosed: boolean = false;
  firstInit = false;

  constructor(private router: Router, private componentService: ComponentServ) {}

  ngOnInit(): void {
    this.isClosed = false;
  }

  ngAfterViewInit(): void {
    requestAnimationFrame(() => {
      if (sessionStorage.getItem("clicked"))
        this.firstInit = true;
    });
  }

  onChanged(value: boolean) {
    requestAnimationFrame(() => {
      this.firstInit = value;
      sessionStorage.setItem("clicked", "1");
    });
  }

  handleViewport(value: 'expand' | 'subtract') {
    if (value === 'expand') {

    } else {

    }
  }

  sideBtnAction() {
    this.isClosed = this.isClosed ? false : true;
  }

  refreshViewport() {
    this.componentService.callRefreshFunction();
  }

  ngOnDestroy(): void {
    sessionStorage.clear();
  }
}
