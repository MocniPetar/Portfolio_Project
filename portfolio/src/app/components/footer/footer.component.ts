import { Component } from '@angular/core';
import { Store } from '@ngrx/store';

@Component({
  selector: 'footer-component',
  imports: [],
  templateUrl: './footer.component.html',
  styleUrl: './footer.component.css',
})
export class FooterComponent {
  constructor(private store: Store) {}
}
