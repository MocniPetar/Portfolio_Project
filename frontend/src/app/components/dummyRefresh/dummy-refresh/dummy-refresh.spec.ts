import { ComponentFixture, TestBed } from '@angular/core/testing';

import { DummyRefresh } from './dummy-refresh';

describe('DummyRefresh', () => {
  let component: DummyRefresh;
  let fixture: ComponentFixture<DummyRefresh>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [DummyRefresh]
    })
    .compileComponents();

    fixture = TestBed.createComponent(DummyRefresh);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
