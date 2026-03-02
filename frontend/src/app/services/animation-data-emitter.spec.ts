import { TestBed } from '@angular/core/testing';

import { AnimationDataEmitter } from './animation-data-emitter';

describe('AnimationDataEmitter', () => {
  let service: AnimationDataEmitter;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(AnimationDataEmitter);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
