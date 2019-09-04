from kivy.app import App
from kivy.lang import Builder
from kivy.uix.widget import Widget


class GMWindow(Widget):
    pass


class GMApp(App):
    title = 'grblmngr'
    
    def build(self):
        Builder.load_file('kv/main.kv')
        return GMWindow()


if __name__ == '__main__':
    GMApp().run()
