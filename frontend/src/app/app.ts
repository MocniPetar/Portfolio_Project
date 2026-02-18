import { Component } from '@angular/core';
import { RouterOutlet } from '@angular/router';
import { NavbarComponent } from './components/navbar/navbar.component';
import { FooterComponent } from './components/footer/footer.component';
import { Intro } from './intro/intro';

@Component({
  selector: 'app-root',
  imports: [NavbarComponent, RouterOutlet, FooterComponent, Intro],
  templateUrl: './app.html',
  styleUrl: './app.less'
})
export class App {
  constructor() {}
}
