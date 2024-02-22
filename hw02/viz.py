#!/usr/bin/env python3

import sys
import random
import pathlib
import tkinter
from PIL import Image, ImageTk


class RescalingCanvas(tkinter.Canvas):
    def __init__(self, parent, positions: list[int], **kwargs):
        super().__init__(parent, **kwargs)
        self.bind("<Configure>", self.resize_callback)
        self.size = min(self.winfo_reqheight(),
                        self.winfo_reqwidth())
        self.raw_images: list[Image.Image] = []
        self.images: list[tkinter.PhotoImage | None] = [None for _ in positions]
        self.positions = positions
        self.draw_chessboard()

    def resize_callback(self, event):
        scale = float(min(event.width, event.height)) / self.size
        self.size = min(event.width, event.height)
        # resize the canvas
        # self.config(width=self.width, height=self.height)
        # rescale all the objects tagged with the "all" tag

        self.scale("all", 0, 0, scale, scale)
        self.draw_queens()

    def draw_chessboard(self):
        n = len(self.positions)
        tile_size = float(self.size) / n

        for xi in range(n):
            for yi in range(xi % 2, n, 2):
                self.create_rectangle(xi * tile_size, yi * tile_size,
                                      (xi+1) * tile_size, (yi+1) * tile_size,
                                      fill='black')
        self.addtag_all('all')

        queen_filenames = list(pathlib.Path(__file__).parent.glob('images/*.jpg'))
        for _ in self.positions:
            filename = random.choice(queen_filenames)
            image = Image.open(filename)
            self.raw_images.append(image)

    def draw_queens(self):
        self.delete('queen')
        n = len(self.positions)
        tile_size = float(self.size) / n

        for x, y in enumerate(self.positions):
            queen = ImageTk.PhotoImage(self.raw_images[x].resize((int(tile_size),) * 2))
            self.images[x] = queen
            self.create_image((x*tile_size, y*tile_size), image=queen,
                              anchor='nw', tag='queen')


def main() -> None:
    positions = [int(p) for p in sys.argv[1].split(',')]

    root = tkinter.Tk()
    # frame = tkinter.Frame(root)
    # frame.pack(fill=tkinter.BOTH, expand=tkinter.YES)

    ws = 600
    canvas = RescalingCanvas(root, positions, bg="white", height=ws, width=ws)

    canvas.pack(fill=tkinter.BOTH, expand=tkinter.YES)

    root.mainloop()


if __name__ == '__main__':
    main()
