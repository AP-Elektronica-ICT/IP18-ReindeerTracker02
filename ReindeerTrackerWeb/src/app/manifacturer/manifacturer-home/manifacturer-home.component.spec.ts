import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { ManifacturerHomeComponent } from './manifacturer-home.component';

describe('ManifacturerHomeComponent', () => {
  let component: ManifacturerHomeComponent;
  let fixture: ComponentFixture<ManifacturerHomeComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ ManifacturerHomeComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(ManifacturerHomeComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
