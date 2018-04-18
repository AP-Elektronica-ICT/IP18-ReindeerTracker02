export interface FilterOptions {
  alive: AliveState,
  battery: BatteryState
}

export enum AliveState {
  all,
  alive,
  dead
}

export enum BatteryState {
  all,
  low,
  high
}
