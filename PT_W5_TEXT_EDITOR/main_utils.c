#include "main_utils.h"

void process_full_command(char *full_line, char **command, char **args)
{
    *command = strtok(full_line, " ");
    char *rest = strtok(NULL, "");
    *args = rest;
}

TypesOfCommand processCommand(char *command)
{
    if (command == NULL)
        return C_INVALID;

    if (!strcmp(command, "addline"))   return C_ADD_LINE;
    if (!strcmp(command, "insert"))    return C_INSERT;
    if (!strcmp(command, "delete"))    return C_DELETE;
    if (!strcmp(command, "deleteline"))    return C_DELETE_LINE;
    if (!strcmp(command, "undo"))      return C_UNDO;
    if (!strcmp(command, "redo"))      return C_REDO;
    if (!strcmp(command, "search"))    return C_SEARCH;
    if (!strcmp(command, "replace"))   return C_REPLACE;
    if (!strcmp(command, "display"))   return C_PRINT;
    if (!strcmp(command, "copy"))      return C_COPY;
    if (!strcmp(command, "cut"))       return C_CUT;
    if (!strcmp(command, "paste"))     return C_PASTE;
    if (!strcmp(command, "openfile"))  return C_OPENFILE;
    if (!strcmp(command, "savefile"))  return C_SAVEFILE;
    if (!strcmp(command, "moveup"))    return C_UP;
    if (!strcmp(command, "movedown"))  return C_DOWN;
    if (!strcmp(command, "moveleft"))  return C_LEFT;
    if (!strcmp(command, "moveright")) return C_RIGHT;
    if (!strcmp(command, "jumptoline"))return C_JUMP_TO_LINE;
    if (!strcmp(command, "exit"))      return C_EXIT;

    return C_INVALID;
}

void printMenu(void)
{
    static char *menu[] =
    {
        "addline <text>",
        "insert <text>",
        "delete",
        "deleteline",
        "undo",
        "redo",
        "search <text>",
        "replace <old> <new>",
        "display",
        "copy",
        "cut",
        "paste",
        "openfile <filename>",
        "savefile",
        "moveup",
        "movedown",
        "moveright",
        "moveleft",
        "jumptoline <Line Number>",
        "exit"
    };

    puts("\n========== MENU ==========");

    for(int i = 0; i < 20; i++)
        printf("%2d. %s\n", i + 1, menu[i]);

    printf("==========================\n");
}

void executeCommand(TextEditor *editor,
                    ClipboardStack *cStack,
                    TypesOfCommand cmd,
                    char *args[],
                    int *running)
{
    switch(cmd)
    {
        case C_ADD_LINE:

            if(*args == NULL)
            {
                printf("[ERROR] No text provided\n");
                break;
            }

            addLine(editor, *args);
            break;

        case C_INSERT:

            if(*args == NULL)
            {
                printf("[ERROR] No text provided\n");
                break;
            }

            insertText(editor, *args);
            break;

        case C_DELETE:
        {
            int n;
            char buf[32];

            printf("Characters to delete : ");

            if(fgets(buf, sizeof(buf), stdin) == NULL)
                break;

            if(sscanf(buf, "%d", &n) != 1)
            {
                printf("Invalid Input\n");
                break;
            }

            deleteCharacters(editor, n);
            break;
        }
        case C_DELETE_LINE:
        {
            int lineNumber = atoi(*args);
            if(deleteLine(editor, lineNumber) == FAILURE)
            {
                printf("[INFO] : Out of Bound Line Number:%d\n", lineNumber);
            }
            break;
        }
        case C_UNDO:
            undoEdit(editor);
            break;

        case C_REDO:
            redoEdit(editor);
            break;

        case C_SEARCH:
        {
            if(*args == NULL)
            {
                printf("Missing search text\n");
                break;
            }

            char choice[8];

            printf("1.Find All\n");
            printf("2.Find Next\n");
            printf("Choice : ");

            fgets(choice, sizeof(choice), stdin);

            if(choice[0] == '1')
            {
                findAll(editor, *args);
            }
            else if(choice[0] == '2')
            {
                int pos, line;

                if(findText(editor, *args, &pos, &line) == SUCCESS)
                    printf("Found at Line %d Column %d\n", line, pos);
                else
                    printf("Not Found\n");
            }

            break;
        }

        case C_REPLACE:
        {
            char *old_text = strtok(*args, " ");
            if(old_text == NULL)
            {
                printf("replace <old> <new>\n");
                break;
            }    
            char *new_text = strtok(NULL, " ");
            if(new_text == NULL)
            {
                printf("replace <old> <new>\n");
                break;
            }

            char ch;

            printf("1.Replace All\n");
            printf("2.Replace Next\n");

            scanf(" %c", &ch);
            getchar();

            if(ch == '1')
                replaceAll(editor, old_text, new_text);
            else if(ch == '2')
                replaceText(editor, old_text, new_text);

            break;
        }

        case C_PRINT:
            print_list(editor);
            break;

        case C_COPY:
        {
            if(navigateToCopyPoint(editor) == FAILURE)
                break;

            int n;
            char buf[32];
            printf("Enter the number of characters to copy : ");
            if(fgets(buf, sizeof(buf), stdin) == NULL)
                break;
            if(sscanf(buf, "%d", &n) != 1)
            {
                printf("[ERROR] : Invalid number\n");
                break;
            }

            if(copy(editor, cStack, n) == FAILURE)
                printf("[INFO] : Nothing to copy at this position\n");

            break;
        }

        case C_CUT:
        {
            if(navigateToCopyPoint(editor) == FAILURE)
                break;

            int n;
            char buf[32];
            printf("Enter the number of characters to cut : ");
            if(fgets(buf, sizeof(buf), stdin) == NULL)
                break;
            if(sscanf(buf, "%d", &n) != 1)
            {
                printf("[ERROR] : Invalid number\n");
                break;
            }

            if(cut(editor, cStack, n) == FAILURE)
                printf("[INFO] : Nothing to cut at this position\n");

            break;
        }

        case C_PASTE:
        {
            if(navigateToCopyPoint(editor) == FAILURE)
                break;
            if(paste(editor, cStack) == FAILURE)
            {
                printf("[INFO] : Clipboard is empty\n");
                break;
            }

            printf("[INFO] : Text pasted successfully\n");
            break;
        }

        case C_OPENFILE:
        {
            if(*args == NULL)
            {
                printf("[ERROR] : No filename provided\n");
                break;
            }
            if(openFile(editor, *args) == FAILURE)
                printf("[ERROR] : Could not open file \"%s\"\n", *args);
            else
                printf("[INFO] : File \"%s\" loaded successfully\n", *args);
            break;
        }

        case C_SAVEFILE:
        {
            if(saveFile(editor) == FAILURE)
                printf("[ERROR] : Could not save file\n");
            else
                printf("[INFO] : File saved as fileSave.txt\n");
            break;
        }

        case C_UP:
            cursorMoveUp(editor);
            break;
        case C_DOWN:
            cursorMoveDown(editor);
            break;
        case C_LEFT:
            cursorMoveLeft(editor);
            break;
        case C_RIGHT:
            cursorMoveRight(editor);
            break;

        case C_JUMP_TO_LINE:
            if(*args == NULL)
            {
                printf("[INFO] : Missing line number\n");
                break;
            }
            if(jumpToLine(editor, atoi(*args)) == FAILURE)
            {
                printf("[ERROR] : Out of Bounds\n");
            }
            break;

        case C_EXIT:
            *running = 0;
            break;

        default:
            printf("Invalid Command\n");
    }
}