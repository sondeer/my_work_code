// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� WRITEARDUINODLL_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// WRITEARDUINODLL_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef WRITEARDUINODLL_EXPORTS
#define WRITEARDUINODLL_API __declspec(dllexport)
#else
#define WRITEARDUINODLL_API __declspec(dllimport)
#endif



WRITEARDUINODLL_API int GeneratePacket(void *buff, void *data, int size);
WRITEARDUINODLL_API UINT8 ClcCheckSum(void *buff, int size);