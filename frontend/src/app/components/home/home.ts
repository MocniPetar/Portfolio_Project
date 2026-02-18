import { Component } from '@angular/core';

@Component({
  selector: 'app-home',
  imports: [],
  templateUrl: './home.html',
  styleUrl: './home.less',
})
export class Home {
  placeholderCards: any[] = [1, 2, 3, 4, 5];

  constructor() {}
}
