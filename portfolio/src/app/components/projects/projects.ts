import { Component } from '@angular/core';

@Component({
  selector: 'app-projects',
  imports: [],
  templateUrl: './projects.html',
  styleUrl: './projects.less',
})
export class Projects {
  placeholderCards: any[] = [1, 2, 3, 4, 5];
  
  constructor() {}
}
