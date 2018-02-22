import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { ManifacturerComponent } from './manifacturer.component';

describe('ManifacturerComponent', () => {
  let component: ManifacturerComponent;
  let fixture: ComponentFixture<ManifacturerComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ ManifacturerComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(ManifacturerComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
