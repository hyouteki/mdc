#include "voyage/export.h"
#include "mdc.h"

static Column canvas;
static char *filename = NULL;

void openFile() {
	filename = "./examples/sample.md";
	Mdc_TokenList *list = Mdc_TokenizeFile(filename);
	while (list) {
		Element *element = NULL;
		switch (list->type) {
		case Mdc_Token_Heading:
			element = &Heading_EleInit(list->content);
			break;
		case Mdc_Token_Text:
			element = &Label_EleInit(list->content);
			break;
		}
		if (element) Column_AddElement(&canvas, element);
		list = list->next;
	}
	Mdc_TokenList_Free(list);
}

void openInBrowser() {
	if (!filename) return;
	Mdc_TokenList *list = Mdc_TokenizeFile(filename);
	FILE *file = fopen("./examples/sample.html", "w");
    Mdc_ConvertToHtml(file, list);
	fclose(file);
    Mdc_TokenList_Free(list);
	system("start ./examples/sample.html");
}

int main() {
	Voyage_Setup(900, 720, 60, "CommonMarkViewer", FLAG_WINDOW_RESIZABLE);
	
	Column sidebar = Column_Init(Vector2Dummy, Vector2Dummy, Voyage_Black);
	Image image = LoadImage("./voyage/images/logo.png");
	Column_AddElement(&sidebar, &ImageContainer_EleInit(&image));
	Column_AddElement(&sidebar, &Button_EleInit("Open file", &openFile));	
	Column_AddElement(&sidebar, &Button_EleInit("Open in Browser", &openInBrowser));	

	canvas = Column_Init(Vector2Dummy, Vector2Dummy, Voyage_DarkBrown);
	
	Row row = Row_Init(Vector2Dummy, Vector2Dummy, 2,
					   (Column *[]){&sidebar, &canvas}, (u32 []){1, 3});
	
	while (!WindowShouldClose()) {
		Row_Resize(&row, Voyage_ScreenDimen);
		Row_ScrollEventHandler(&row);	

		BeginDrawing();
		ClearBackground(BLACK);
		Row_Draw(row);
		EndDrawing();
	}
	CloseWindow();
	Row_Free(&row);
	
	return 0;
}
