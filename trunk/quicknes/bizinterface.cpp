#include <cstdlib>
#include <cstring>
#include "nes_emu/Nes_Emu.h"

// simulate the write so we'll know how long the buffer needs to be
class Sim_Writer : public Data_Writer
{
	long size_;
public:
	Sim_Writer():size_(0) { }
	error_t write(const void *, long size)
	{
		size_ += size;
		return 0;
	}
	long size() const { return size_; }
};


#define EXPORT extern "C" __declspec(dllexport)

EXPORT void qn_setup_mappers()
{
	register_optional_mappers();
}

EXPORT Nes_Emu *qn_new()
{
	return new Nes_Emu();
}

EXPORT void qn_delete(Nes_Emu *e)
{
	delete e;
}

EXPORT const char *qn_loadines(Nes_Emu *e, const void *data, int length)
{
	Mem_File_Reader r(data, length);
	Auto_File_Reader a(r);
	return e->load_ines(a);
}

EXPORT const char *qn_set_sample_rate(Nes_Emu *e, int rate)
{
	const char *ret = e->set_sample_rate(rate);
	if (!ret)
		e->set_equalizer(Nes_Emu::nes_eq);
	return ret;
}

EXPORT void qn_get_image_dimensions(Nes_Emu *e, int *width, int *height)
{
	if (width)
		*width = e->buffer_width;
	if (height)
		*height = e->buffer_height();
}

EXPORT void qn_set_pixels(Nes_Emu *e, void *dest, int pitch)
{
	e->set_pixels(dest, pitch);
}

EXPORT const char *qn_emulate_frame(Nes_Emu *e, int pad1, int pad2)
{
	return e->emulate_frame(pad1, pad2);
}

EXPORT void qn_blit(Nes_Emu *e, char *dest)
{
	// what is the point of the 256 color bitmap and the dynamic color allocation to it?
	// why not just render directly to a 512 color bitmap with static palette positions?

	const unsigned char *src = e->frame().pixels;
	const int srcpitch = e->frame().pitch;
	const unsigned char *srcend = src + e->image_height * srcpitch;

	const short *lut = e->frame().palette;
	const Nes_Emu::rgb_t *colors = e->nes_colors;

	for (; src < srcend; src += srcpitch)
	{
		for (int i = 0; i < 256; i++)
		{
			const Nes_Emu::rgb_t *c = colors + lut[src[i]];
			*dest++ = c->blue;
			*dest++ = c->green;
			*dest++ = c->red;
			*dest++ = 0xff;
		}
	}
}

EXPORT int qn_get_joypad_read_count(Nes_Emu *e)
{
	return e->frame().joypad_read_count;
}

EXPORT void qn_get_audio_info(Nes_Emu *e, int *sample_count, int *chan_count)
{
	if (sample_count)
		*sample_count = e->frame().sample_count;
	if (chan_count)
		*chan_count = e->frame().chan_count;
}

EXPORT int qn_read_audio(Nes_Emu *e, short *dest, int max_samples)
{
	return e->read_samples(dest, max_samples);
}

EXPORT void qn_reset(Nes_Emu *e, int hard)
{
	e->reset(hard);
}

EXPORT const char *qn_state_size(Nes_Emu *e, int *size)
{
	Sim_Writer w;
	Auto_File_Writer a(w);
	const char *ret = e->save_state(a);
	if (size)
		*size = w.size();
	return ret;
}

EXPORT const char *qn_state_save(Nes_Emu *e, void *dest, int size)
{
	Mem_Writer w(dest, size, 0);
	Auto_File_Writer a(w);
	const char *ret = e->save_state(a);
	if (!ret && w.size() != size)
		return "Buffer Underrun!";
	return ret;
}

EXPORT const char *qn_state_load(Nes_Emu *e, const void *src, int size)
{
	Mem_File_Reader r(src, size);
	Auto_File_Reader a(r);
	return e->load_state(a);
}

EXPORT int qn_has_battery_ram(Nes_Emu *e)
{
	return e->has_battery_ram();
}

EXPORT const char *qn_battery_ram_size(Nes_Emu *e, int *size)
{
	Sim_Writer w;
	Auto_File_Writer a(w);
	const char *ret = e->save_battery_ram(a);
	if (size)
		*size = w.size();
	return ret;
}

EXPORT const char *qn_battery_ram_save(Nes_Emu *e, void *dest, int size)
{
	Mem_Writer w(dest, size, 0);
	Auto_File_Writer a(w);
	const char *ret = e->save_battery_ram(a);
	if (!ret && w.size() != size)
		return "Buffer Underrun!";
	return ret;
}

EXPORT const char *qn_battery_ram_load(Nes_Emu *e, const void *src, int size)
{
	Mem_File_Reader r(src, size);
	Auto_File_Reader a(r);
	return e->load_battery_ram(a);
}

EXPORT const char *qn_battery_ram_clear(Nes_Emu *e)
{
	int size = 0;
	const char *ret = qn_battery_ram_size(e, &size);
	if (ret)
		return ret;
	void *data = std::malloc(size);
	if (!data)
		return "Out of Memory!";
	std::memset(data, 0xff, size);
	ret = qn_battery_ram_load(e, data, size);
	std::free(data);
	return ret;
}

EXPORT void qn_set_sprite_limit(Nes_Emu *e, int n)
{
	e->set_sprite_mode((Nes_Emu::sprite_mode_t)n);
}
