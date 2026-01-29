export interface Pages {
    page: Page;
}

export interface Page {
    label: string;
    path: string;
    modal: boolean;
}

export interface linkManager {
  id: number,
  isAnimated: boolean
}