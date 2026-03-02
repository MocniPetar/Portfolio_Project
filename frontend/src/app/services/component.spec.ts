import { TestBed } from '@angular/core/testing';

import { ComponentServ } from './component';

describe('ComponentServ', () => {
  let service: ComponentServ;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(ComponentServ);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
