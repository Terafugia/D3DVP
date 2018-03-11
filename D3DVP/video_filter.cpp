//----------------------------------------------------------------------------------
//		�T���v���r�f�I�t�B���^(�t�B���^�v���O�C��)  for AviUtl ver0.99e�ȍ~
//----------------------------------------------------------------------------------
#include <windows.h>
#undef max
#undef min

#include <algorithm>
#include <string>
#include <mutex>
#include <condition_variable>

// avisynth�Ƀ����N���Ă���̂�
#define AVS_LINKAGE_DLLIMPORT
#include "avisynth.h"
#pragma comment(lib, "avisynth.lib")

#include "filter.h"

// �f�o�b�O�p
#define THROUGH_VIDEO 0

HMODULE g_DllHandle;

//---------------------------------------------------------------------
//		�t�B���^�\���̒�`
//---------------------------------------------------------------------
#define	TRACK_N	2														//	�g���b�N�o�[�̐�
TCHAR	*track_name[] =		{	"Preset", "Threads"	};	//	�g���b�N�o�[�̖��O
int		track_default[] =	{	2, 4 };	//	�g���b�N�o�[�̏����l
int		track_s[] =			{	-4, 0 };	//	�g���b�N�o�[�̉����l
int		track_e[] =			{	6, 16 };	//	�g���b�N�o�[�̏���l

#define	CHECK_N	4													//	�`�F�b�N�{�b�N�X�̐�
TCHAR	*check_name[] = 	{	"2�{fps���i2�{fps�œ��͂��Ăˁj", "YUV420�ŏ���", "BFF", "�������Ȃ��i�f�o�b�O�p�j" };				//	�`�F�b�N�{�b�N�X�̖��O
int		check_default[] = 	{	1, 1, 0, 0			};				//	�`�F�b�N�{�b�N�X�̏����l (�l��0��1)

FILTER_DLL filter = {
	FILTER_FLAG_EX_INFORMATION,	//	�t�B���^�̃t���O
								//	FILTER_FLAG_ALWAYS_ACTIVE		: �t�B���^����ɃA�N�e�B�u�ɂ��܂�
								//	FILTER_FLAG_CONFIG_POPUP		: �ݒ���|�b�v�A�b�v���j���[�ɂ��܂�
								//	FILTER_FLAG_CONFIG_CHECK		: �ݒ���`�F�b�N�{�b�N�X���j���[�ɂ��܂�
								//	FILTER_FLAG_CONFIG_RADIO		: �ݒ�����W�I�{�^�����j���[�ɂ��܂�
								//	FILTER_FLAG_EX_DATA				: �g���f�[�^��ۑ��o����悤�ɂ��܂��B
								//	FILTER_FLAG_PRIORITY_HIGHEST	: �t�B���^�̃v���C�I���e�B����ɍŏ�ʂɂ��܂�
								//	FILTER_FLAG_PRIORITY_LOWEST		: �t�B���^�̃v���C�I���e�B����ɍŉ��ʂɂ��܂�
								//	FILTER_FLAG_WINDOW_THICKFRAME	: �T�C�Y�ύX�\�ȃE�B���h�E�����܂�
								//	FILTER_FLAG_WINDOW_SIZE			: �ݒ�E�B���h�E�̃T�C�Y���w��o����悤�ɂ��܂�
								//	FILTER_FLAG_DISP_FILTER			: �\���t�B���^�ɂ��܂�
								//	FILTER_FLAG_EX_INFORMATION		: �t�B���^�̊g������ݒ�ł���悤�ɂ��܂�
								//	FILTER_FLAG_NO_CONFIG			: �ݒ�E�B���h�E��\�����Ȃ��悤�ɂ��܂�
								//	FILTER_FLAG_AUDIO_FILTER		: �I�[�f�B�I�t�B���^�ɂ��܂�
								//	FILTER_FLAG_RADIO_BUTTON		: �`�F�b�N�{�b�N�X�����W�I�{�^���ɂ��܂�
								//	FILTER_FLAG_WINDOW_HSCROLL		: �����X�N���[���o�[�����E�B���h�E�����܂�
								//	FILTER_FLAG_WINDOW_VSCROLL		: �����X�N���[���o�[�����E�B���h�E�����܂�
								//	FILTER_FLAG_IMPORT				: �C���|�[�g���j���[�����܂�
								//	FILTER_FLAG_EXPORT				: �G�N�X�|�[�g���j���[�����܂�
	0,0,						//	�ݒ�E�C���h�E�̃T�C�Y (FILTER_FLAG_WINDOW_SIZE�������Ă��鎞�ɗL��)
	"QTGMC(Avisynth)",			//	�t�B���^�̖��O
	TRACK_N,					//	�g���b�N�o�[�̐� (0�Ȃ疼�O�����l����NULL�ł悢)
	track_name,					//	�g���b�N�o�[�̖��O�S�ւ̃|�C���^
	track_default,				//	�g���b�N�o�[�̏����l�S�ւ̃|�C���^
	track_s,track_e,			//	�g���b�N�o�[�̐��l�̉������ (NULL�Ȃ�S��0�`256)
	CHECK_N,					//	�`�F�b�N�{�b�N�X�̐� (0�Ȃ疼�O�����l����NULL�ł悢)
	check_name,					//	�`�F�b�N�{�b�N�X�̖��O�S�ւ̃|�C���^
	check_default,				//	�`�F�b�N�{�b�N�X�̏����l�S�ւ̃|�C���^
	func_proc,					//	�t�B���^�����֐��ւ̃|�C���^ (NULL�Ȃ�Ă΂�܂���)
  func_init,						//	�J�n���ɌĂ΂��֐��ւ̃|�C���^ (NULL�Ȃ�Ă΂�܂���)
  func_exit,						//	�I�����ɌĂ΂��֐��ւ̃|�C���^ (NULL�Ȃ�Ă΂�܂���)
  func_update,						//	�ݒ肪�ύX���ꂽ�Ƃ��ɌĂ΂��֐��ւ̃|�C���^ (NULL�Ȃ�Ă΂�܂���)
	NULL,						//	�ݒ�E�B���h�E�ɃE�B���h�E���b�Z�[�W���������ɌĂ΂��֐��ւ̃|�C���^ (NULL�Ȃ�Ă΂�܂���)
	NULL,NULL,					//	�V�X�e���Ŏg���܂��̂Ŏg�p���Ȃ��ł�������
	NULL,						//  �g���f�[�^�̈�ւ̃|�C���^ (FILTER_FLAG_EX_DATA�������Ă��鎞�ɗL��)
	NULL,						//  �g���f�[�^�T�C�Y (FILTER_FLAG_EX_DATA�������Ă��鎞�ɗL��)
	"�T���v���t�B���^ version 0.06 by �j�d�m����",
								//  �t�B���^���ւ̃|�C���^ (FILTER_FLAG_EX_INFORMATION�������Ă��鎞�ɗL��)
  func_save_start,						//	�Z�[�u���J�n����钼�O�ɌĂ΂��֐��ւ̃|�C���^ (NULL�Ȃ�Ă΂�܂���)
  func_save_end,						//	�Z�[�u���I���������O�ɌĂ΂��֐��ւ̃|�C���^ (NULL�Ȃ�Ă΂�܂���)
};


//---------------------------------------------------------------------
//		�t�B���^�\���̂̃|�C���^��n���֐�
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport) * __stdcall GetFilterTable( void )
{
	return &filter;
}
//	���L�̂悤�ɂ����1��auf�t�@�C���ŕ����̃t�B���^�\���̂�n�����Ƃ��o���܂�
/*
FILTER_DLL *filter_list[] = {&filter,&filter2,NULL};
EXTERN_C FILTER_DLL __declspec(dllexport) ** __stdcall GetFilterTableList( void )
{
	return (FILTER_DLL **)&filter_list;
}
*/

template<typename T>
T clamp(T n, T min, T max)
{
  n = n > max ? max : n;
  return n < min ? min : n;
}

void ConvertYC48toYUY2(PVideoFrame& dst, const PIXEL_YC* src, int w, int h, int max_w)
{
  BYTE* dstptr = dst->GetWritePtr();
  int pitch = dst->GetPitch();

  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; x += 2) {
      short y0 = src[x + y * max_w].y;
      short y1 = src[x + 1 + y * max_w].y;
      short cb = src[x + y * max_w].cb;
      short cr = src[x + y * max_w].cr;

      BYTE Y0 = clamp(((y0 * 219 + 383) >> 12) + 16, 0, 255);
      BYTE Y1 = clamp(((y1 * 219 + 383) >> 12) + 16, 0, 255);
      BYTE U = clamp((((cb + 2048) * 7 + 66) >> 7) + 16, 0, 255);
      BYTE V = clamp((((cr + 2048) * 7 + 66) >> 7) + 16, 0, 255);

      dstptr[x * 2 + 0 + y * pitch] = Y0;
      dstptr[x * 2 + 1 + y * pitch] = U;
      dstptr[x * 2 + 2 + y * pitch] = Y1;
      dstptr[x * 2 + 3 + y * pitch] = V;
    }
  }
}

void ConvertYUY2toYC48(PIXEL_YC* dst, PVideoFrame& src, int w, int h, int max_w)
{
  const BYTE* srcptr = src->GetReadPtr();
  int pitch = src->GetPitch();

  for (int y = 0; y < h; ++y) {

    // �܂���UV�͍��ɂ��̂܂ܓ����
    for (int x = 0, x2 = 0; x < w; x += 2, ++x2) {
      BYTE Y0 = srcptr[x * 2 + 0 + y * pitch];
      BYTE U = srcptr[x * 2 + 1 + y * pitch];
      BYTE Y1 = srcptr[x * 2 + 2 + y * pitch];
      BYTE V = srcptr[x * 2 + 3 + y * pitch];

      short y0 = ((Y0 * 1197) >> 6) - 299;
      short y1 = ((Y1 * 1197) >> 6) - 299;
      short cb = ((U - 128) * 4681 + 164) >> 8;
      short cr = ((V - 128) * 4681 + 164) >> 8;

      dst[x + y * max_w].y = y0;
      dst[x + 1 + y * max_w].y = y1;
      dst[x + y * max_w].cb = cb;
      dst[x + y * max_w].cr = cr;
    }

    // UV�̓����Ă��Ȃ��Ƃ������
    short cb0 = dst[y * max_w].cb;
    short cr0 = dst[y * max_w].cr;
    for (int x = 0; x < w - 2; x += 2) {
      short cb2 = dst[x + 2 + y * max_w].cb;
      short cr2 = dst[x + 2 + y * max_w].cr;
      dst[x + 1 + y * max_w].cb = (cb0 + cb2) >> 1;
      dst[x + 1 + y * max_w].cr = (cr0 + cr2) >> 1;
      cb0 = cb2;
      cr0 = cr2;
    }
    dst[w - 1 + y * max_w].cb = cb0;
    dst[w - 1 + y * max_w].cr = cr0;
  }
}

#define SOURCE_FILTER_NAME "AviUtlFilterSource"

class QTGMCTest
{
  struct Config {
    int preset;
    int threads;
    bool doublefps;
    bool yv12;
    bool bff;
    bool through;

    bool operator==(Config o) {
      return preset == o.preset &&
        threads == o.threads &&
        doublefps == o.doublefps &&
        yv12 == o.yv12 &&
        bff == o.bff &&
        through == o.through;
    }
    bool operator!=(const Config& o) {
      return !(*this == o);
    }
  };

  Config conf;

  IScriptEnvironment2* env_;

  PClip filter_;

  std::mutex mutex_;
  std::condition_variable getframe_cond_;
  std::condition_variable proc_cond_;
  bool allowGetFrame_;
  bool cancelGetFrame_;
  int enterCount;

  void CreateFilter() {
    const char* presets[] = {
      "Placebo",
      "Very Slow",
      "Slower",
      "Slow",
      "Medium",
      "Fast",
      "Faster",
      "Very Fast",
      "Super Fast",
      "Ultra Fast",
      "Draft"
    };

    env_ = CreateScriptEnvironment2();

    // �������v���O�C���Ƃ��ă��[�h
    AVSValue result;
    std::string modulepath = GetModulePath();
    env_->LoadPlugin(modulepath.c_str(), true, &result);

    AVSValue last = env_->Invoke(SOURCE_FILTER_NAME, AVSValue(nullptr, 0), 0);
    if (conf.yv12) {
      AVSValue args[] = { last, true };
      const char* arg_names[] = { nullptr, "interlaced", };
      last = env_->Invoke("ConvertToYV12", AVSValue(args, 2), arg_names);
    }
    if (conf.through == false) {
      if (conf.doublefps) {
        // 2�{fps���̏ꍇ�́A�\�[�X��2�{fps����Ă���̂Ŗ߂�
        AVSValue args[] = { last, 2, 0 };
        last = env_->Invoke("SelectEvery", AVSValue(args, 3), 0);
      }
      AVSValue args[] = { last, presets[conf.preset + 4], (conf.doublefps ? 1 : 2) };
      const char* arg_names[] = { nullptr, "Preset", "FPSDivisor" };
      last = env_->Invoke("QTGMC", AVSValue(args, 3), arg_names);
    }
    if (conf.yv12) {
      AVSValue args[] = { last };
      last = env_->Invoke("ConvertToYUY2", AVSValue(args, 1), 0);
    }
    if (conf.threads > 0) {
      AVSValue args[] = { last, conf.threads };
      last = env_->Invoke("Prefetch", AVSValue(args, 2), 0);
    }
    filter_ = last.AsClip();
  }

  Config ReadConfig() {
    Config c;
    c.preset = fp_->track[0];
    c.threads = fp_->track[1];
    c.doublefps = (fp_->check[0] != 0);
    c.yv12 = (fp_->check[1] != 0);
    c.bff = (fp_->check[2] != 0);
    c.through = (fp_->check[3] != 0);
    return c;
  }

  std::string GetModulePath() {
    char buf[MAX_PATH];
    GetModuleFileName(g_DllHandle, buf, MAX_PATH);
    return buf;
  }

  void EnterProc()
  {
    // GetFrame�ő҂��Ă���l���N����
    std::lock_guard<std::mutex> lock(mutex_);
    allowGetFrame_ = true;
    getframe_cond_.notify_all();
  }

  void ExitProc()
  {
    std::unique_lock<std::mutex> lock(mutex_);
    while (enterCount > 0) {
      // GetFrame���甲����܂ő҂�
      proc_cond_.wait(lock);
    }
    allowGetFrame_ = false;
  }

  class ProcGuard
  {
    QTGMCTest* pThis;
  public:
    ProcGuard(QTGMCTest* pThis) : pThis(pThis)
    {
      pThis->EnterProc();
    }
    ~ProcGuard()
    {
      pThis->ExitProc();

      // ���S�̂��߃A�N�Z�X�o���Ȃ��悤�ɂ��Ă���
      pThis->fp_ = nullptr;
      pThis->fpip_ = nullptr;
    }
  };

public:

  FILTER *fp_;
  FILTER_PROC_INFO *fpip_;

  QTGMCTest()
    : conf()
    , env_(nullptr)
    , allowGetFrame_(false)
    , cancelGetFrame_(false)
    , enterCount(0)
  {
    //
  }

  ~QTGMCTest()
  {
    DeleteFilter();
  }

  void DeleteFilter() {
    if (env_ != nullptr) {
      {
        // GetFrame�ő҂��Ă���l���N����
        std::lock_guard<std::mutex> lock(mutex_);
        cancelGetFrame_ = true;
      }
      getframe_cond_.notify_all();

      filter_ = nullptr;
      env_->DeleteScriptEnvironment();
      env_ = nullptr;
      allowGetFrame_ = false;
      cancelGetFrame_ = false;
    }
  }

  BOOL Proc(FILTER *fp, FILTER_PROC_INFO *fpip)
  {
    fp_ = fp;
    fpip_ = fpip;

    try {
      // �ݒ肪�ς���Ă������蒼��
      Config newconf = ReadConfig();
      if (conf != newconf) {
        DeleteFilter();
        conf = newconf;
      }
      if (!filter_) {
        CreateFilter();
      }

      if (!fp->exfunc->set_ycp_filtering_cache_size(fp, fpip->max_w, fpip->h, 3, NULL)) {
        MessageBox(fp->hwnd, "�L���b�V���ݒ�Ɏ��s", "QTGMCTest", MB_OK);
        return FALSE;
      }

      ProcGuard guard(this);

      PVideoFrame frame = filter_->GetFrame(fpip_->frame, env_);

      // YC48�ɕϊ�
      ConvertYUY2toYC48(fpip_->ycp_temp, frame, fpip_->w, fpip_->h, fpip_->max_w);

      // temp��edit�����ւ�
      std::swap(fpip_->ycp_edit, fpip_->ycp_temp);
    }
    catch(AvisynthError& err) {
      MessageBox(fp->hwnd, err.msg, "QTGMCTest", MB_OK);
      return FALSE;
    }
    catch (IScriptEnvironment::NotFound&) {
      MessageBox(fp->hwnd, "�֐���������܂���", "QTGMCTest", MB_OK);
      return FALSE;
    }
    return TRUE;
  }

  // �߂�l: �L�����Z�����ꂽ��
  bool EnterGetFrame()
  {
    std::unique_lock<std::mutex> lock(mutex_);
    while (allowGetFrame_ == false && cancelGetFrame_ == false) {
      // proc���Ă΂��܂ő҂�
      getframe_cond_.wait(lock);
    }
    ++enterCount;
    return cancelGetFrame_;
  }

  void ExitGetFrame()
  {
    std::unique_lock<std::mutex> lock(mutex_);

    //if (cancelGetFrame_ == false) {
    //  if (fp_ == nullptr || fpip_ == nullptr) {
    //    MessageBox(NULL, "�o�O���Ă܂��I", "QTGMCTest", MB_OK);
    //  }
    //}

    --enterCount;
    proc_cond_.notify_all();
  }

  const Config& GetConf() { return conf; }
};

class AviUtlFilterSource : public IClip
{
  QTGMCTest* pThis;
  VideoInfo vi;
  bool istff;

  class GetFrameGuard
  {
    QTGMCTest* pThis;
    bool isCanceled;
  public:
    GetFrameGuard(QTGMCTest* pThis)
      : pThis(pThis)
    {
      isCanceled = pThis->EnterGetFrame();
    }
    ~GetFrameGuard()
    {
      pThis->ExitGetFrame();
    }
    bool IsCanceled()
    {
      return isCanceled;
    }
  };

public:
  AviUtlFilterSource(QTGMCTest* pThis)
    : pThis(pThis)
    , vi()
  {
    istff = !pThis->GetConf().bff;

    vi.width = pThis->fpip_->w;
    vi.height = pThis->fpip_->h;
    vi.pixel_type = VideoInfo::CS_YUY2;
    vi.num_frames = pThis->fpip_->frame_n;

    // FPS�͎g��Ȃ��̂œK���Ȓl�ɂ��Ă���
    vi.SetFPS(30000, 1001);
  }

  PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env)
  {
    // n���͈͊O�ɍs���Ȃ��悤�ɂ���
    n = clamp(n, 0, vi.num_frames - 1);

    GetFrameGuard guard(pThis);

    if (guard.IsCanceled()) {
      return env->NewVideoFrame(vi);
    }

    // AviUtl����t���[���擾
    PIXEL_YC* frame_ptr;
    if (n == pThis->fpip_->frame) {
      // ���̃t���[��
      frame_ptr = pThis->fpip_->ycp_edit;
    }
    else {
      // ���̃t���[���łȂ�
      frame_ptr = pThis->fp_->exfunc->get_ycp_filtering_cache_ex(pThis->fp_, pThis->fpip_->editp, n, NULL, NULL);
      if (frame_ptr == NULL) {
        // �������s��
        env->ThrowError("�������s��");
      }
    }

    PVideoFrame dst = env->NewVideoFrame(vi);

    // YUY2�ɕϊ�
    ConvertYC48toYUY2(dst, frame_ptr, pThis->fpip_->w, pThis->fpip_->h, pThis->fpip_->max_w);

    return dst;
  }

  void __stdcall GetAudio(void* buf, __int64 start, __int64 count, IScriptEnvironment* env) { }
  const VideoInfo& __stdcall GetVideoInfo() { return vi; }
  bool __stdcall GetParity(int n) { return istff; }
  int __stdcall SetCacheHints(int cachehints, int frame_range) { return 0; };

  static AVSValue Create(AVSValue args, void* user_data, IScriptEnvironment* env) {
    return new AviUtlFilterSource(static_cast<QTGMCTest*>(user_data));
  }
};

QTGMCTest* g_filter;

BOOL func_init(FILTER *fp)
{
  g_filter = new QTGMCTest();
  return TRUE;
}

BOOL func_exit(FILTER *fp)
{
  delete g_filter;
  return TRUE;
}

BOOL func_update(FILTER *fp, int status)
{
  return TRUE;
}

BOOL func_save_start(FILTER *fp, int s, int e, void *editp)
{
  if (g_filter != nullptr) {
    // �S�~�t���[����������̂�����邽�߃L���b�V���N���A���Ă���
    g_filter->DeleteFilter();
  }
  return TRUE;
}

BOOL func_save_end(FILTER *fp, void *editp)
{
  return TRUE;
}

//---------------------------------------------------------------------
//		�t�B���^�����֐�
//---------------------------------------------------------------------
BOOL func_proc( FILTER *fp,FILTER_PROC_INFO *fpip )
{
  return g_filter->Proc(fp, fpip);
}

extern "C" __declspec(dllexport) const char* __stdcall AvisynthPluginInit3(IScriptEnvironment* env, const AVS_Linkage* const vectors) {
  // ���ڃ����N���Ă���̂�vectors���i�[����K�v�͂Ȃ�

  env->AddFunction(SOURCE_FILTER_NAME, "", AviUtlFilterSource::Create, g_filter);
  
  return "QTGMCAviUtlPlugin";
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
  if (dwReason == DLL_PROCESS_ATTACH) g_DllHandle = hModule;
  return TRUE;
}
