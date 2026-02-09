import { AfterViewInit, Component, ElementRef, Input, OnInit, QueryList, ViewChild, ViewChildren } from '@angular/core';
import { Store } from '@ngrx/store';
import { firstValueFrom } from 'rxjs';
import { selectLabels } from '../../states/labels/labels.selectors';
import { selectSettings } from '../../states/appsettings/settings.selectors';
import { Pages, Page, linkManager } from '../../models/menu';
import { Router } from '@angular/router';
import { MatIconModule } from "@angular/material/icon";

@Component({
  selector: 'navbar',
  imports: [MatIconModule],
  templateUrl: './navbar.component.html',
  styleUrl: './navbar.component.less',
})

export class NavbarComponent implements OnInit, AfterViewInit
{
  @ViewChildren('link', {read: ElementRef}) links!: QueryList<ElementRef<HTMLElement>>;
  @ViewChild('bottomScroll', { static: true }) bottomScroll!: ElementRef<HTMLDivElement>;
  @Input() menus: Pages[] = [];

  nameLabel: string = "";
  occupationLabel: string = "";

  private linkAnimationManager: linkManager[] = [];
  
  constructor(private store: Store, private router: Router) {}

  ngOnInit(): void {

    this.linkAnimationManager = [
      {id: 0, isAnimated: false},
      {id: 1, isAnimated: false},
      {id: 2, isAnimated: false},
      {id: 3, isAnimated: false}
    ];

    firstValueFrom<any>(this.store.select(selectLabels))
      .then(labels => {
        const { header } = labels;
        this.nameLabel = header.nameLabel;
        this.occupationLabel = header.occupationLabel;
      })
      .catch(error => {
        throw error;
      });

    firstValueFrom<any>(this.store.select(selectSettings))
      .then(settings => {
        const { pages } = settings;
        this.menus = pages; 
      })
      .catch(error => {
        throw error;
      });
  }

  ngAfterViewInit(): void {
    this.checkLinks();

    this.links.changes.subscribe(() => {
      this.checkLinks();
    });

    this.setupScrollEffect();
  }

  private checkLinks() {
    if (this.links.length > 0) {
      this.links.forEach(link => {
        link.nativeElement.addEventListener("animationstart", (event: AnimationEvent) =>  {
          let idx = Number(link.nativeElement.id);
          this.linkAnimationManager[idx].isAnimated = this.listener(event);
        });
        link.nativeElement.addEventListener("animationend", (event: AnimationEvent) => {
          let idx = Number(link.nativeElement.id);
          this.linkAnimationManager[idx].isAnimated = this.listener(event);
        });
      });
    } 
  }

  private setupScrollEffect() {
    
  }

  navLinkItemAnimation(event: any) {
    if (!this.linkAnimationManager[event.srcElement.id as number].isAnimated) {
      if (event.srcElement.classList.contains('animate')) {
        event.srcElement.classList.remove('animate');
        void event.srcElement.offsetWidth;
      }
      event.srcElement.classList.add('animate');
    }
  }

  listener(event: any) {
    switch (event.type) {
      case "animationstart":
        return true;
      case "animationend":
        return false;
    }

    return false;
  }

  handleNavigation(page: Page) {
    this.router.navigate([page.path])
  }
}
