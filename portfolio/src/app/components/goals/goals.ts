import { Component } from '@angular/core';

@Component({
  selector: 'app-goals',
  imports: [],
  templateUrl: './goals.html',
  styleUrl: './goals.less',
})
export class Goals {
  placeholderCards: any[] = [1, 2, 3, 4, 5];
  
  constructor() {}
}
