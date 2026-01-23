import { Component, Input, OnInit } from '@angular/core';
import { Store } from '@ngrx/store';
import { firstValueFrom } from 'rxjs';
import { selectLabels } from '../../states/labels/labels.selectors';
import { selectSettings } from '../../states/appsettings/settings.selectors';
import { config } from 'process';
import { error } from 'console';
import { Pages, Page } from '../../models/menu';
import { Router } from '@angular/router';

@Component({
  selector: 'navbar',
  imports: [],
  templateUrl: './navbar.component.html',
  styleUrl: './navbar.component.less',
})
export class NavbarComponent implements OnInit{
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

  handleNavigation(page: Page) {
    this.router.navigate([page.path])
  }
}
