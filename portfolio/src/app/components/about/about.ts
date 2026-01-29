import { Component } from '@angular/core';

@Component({
  selector: 'app-about',
  imports: [],
  templateUrl: './about.html',
  styleUrl: './about.less',
})
export class About {
  placeholderCards: any[] = [1, 2, 3, 4, 5];
  
  constructor() {}
}
