import { AfterViewInit, Component, ElementRef, Input, OnInit, ViewChild } from '@angular/core';
import { Store } from '@ngrx/store';
import { firstValueFrom } from 'rxjs';
import { selectLabels } from '../../states/labels/labels.selectors';
import { selectSettings } from '../../states/appsettings/settings.selectors';
import { config } from 'process';
import { error } from 'console';
import { Pages, Page } from '../../models/menu';
import { Router } from '@angular/router';
import { MatIconModule } from "@angular/material/icon";

@Component({
  selector: 'navbar',
  imports: [MatIconModule],
  templateUrl: './navbar.component.html',
  styleUrl: './navbar.component.less',
})
export class NavbarComponent implements OnInit, AfterViewInit{
  @Input() menus: Pages[] = [];

  nameLabel: string = "";
  occupationLabel: string = "";
  
  constructor(private store: Store, private router: Router) {}

  ngOnInit(): void {
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
  }

  navLinkItemAnimation(event: any) {
    if (event.srcElement.classList.contains('animate')) {
      event.srcElement.classList.remove('animate');
      void event.srcElement.offsetWidth;
    }
    event.srcElement.classList.add('animate');
  }

  listener(event: any) {
    switch (event.type) {
      case "animationstart":
        console.log(`Started: elapsed time is ${event.elapsedTime}`);
        break;
      case "animationend":
        console.log(`Started: elapsed time is ${event.elapsedTime}`)
        break;
      case "animationiteration":
        console.log(`Started: elapsed time is ${event.elapsedTime}`)
        break;
    }
  }

  handleNavigation(page: Page) {
    this.router.navigate([page.path])
  }
}
