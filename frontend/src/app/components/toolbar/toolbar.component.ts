import { Component, ElementRef, Input, OnInit, Output, ViewChild, EventEmitter } from '@angular/core';
import { Store } from '@ngrx/store';
import { firstValueFrom } from 'rxjs';
import { selectLabels } from '../../states/labels/labels.selectors';
import { selectSettings } from '../../states/appsettings/settings.selectors';
import { Pages } from '../../models/menu';
import { Router } from '@angular/router';
import { MatIconModule } from "@angular/material/icon";
import { ReactiveFormsModule, FormBuilder, Validators, FormGroup } from '@angular/forms';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatInputModule } from '@angular/material/input';
import { AnimationDataEmitter } from '../../services/animation-data-emitter';
import { animationData } from '../../models/animation';

@Component({
  selector: 'toolbar',
  imports: [MatIconModule, ReactiveFormsModule, MatFormFieldModule, MatInputModule],
  templateUrl: './toolbar.component.html',
  styleUrl: './toolbar.component.less',
})

export class ToolbarComponent implements OnInit
{
  @Input() menus: Pages[] = [];
  @Output() dataEmitter = new EventEmitter<string>();

  animationConfigForm!: FormGroup;
  
  constructor(
    private fb: FormBuilder, 
    private store: Store, 
    private router: Router,
    private dataService: AnimationDataEmitter
  ) {}

  ngOnInit(): void {
    firstValueFrom<any>(this.store.select(selectLabels))
      .then(labels => {
        const { toolbar_leabels } = labels;
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

      this.animationConfigForm = this.fb.group({
        animationText: [''],
        animationDuration: [''],
        animationDelay: [''],
        continueText: ['']
      });
  }

  submitNewConfig() {
    let data: animationData = {
      mainText: this.animationConfigForm.get('animationText')?.value,
      continuteText: this.animationConfigForm.get('continueText')?.value,
      animationDuration: Number(this.animationConfigForm.get('animationDuration')?.value),
      animationDelay: Number(this.animationConfigForm.get('animationDelay')?.value),
    };
    this.dataService.changeAnimationData(data);
    this.dataService.callIntroAnimationFunction();
  }

  emitAction(value: string) { this.dataEmitter.emit(value); }
}
