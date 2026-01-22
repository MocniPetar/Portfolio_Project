import { Component, Input, OnInit } from '@angular/core';
import { Store } from '@ngrx/store';
import { firstValueFrom } from 'rxjs';
import { selectLabels } from '../../states/labels/labels.selectors';
import { selectSettings } from '../../states/appsettings/settings.selectors';
import { config } from 'process';
import { error } from 'console';

@Component({
  selector: 'navbar',
  imports: [],
  templateUrl: './navbar.component.html',
  styleUrl: './navbar.component.css',
})
export class NavbarComponent implements OnInit{
  @Input() menus: any = [];
  
  constructor(private store: Store) {}

  ngOnInit(): void {
    firstValueFrom<any>(this.store.select(selectLabels))
      .then(labels => {
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

  handleNavigation(menu: any) {

  }
}
