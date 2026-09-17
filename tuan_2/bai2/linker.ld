MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 64K /* Vùng nhớ FLASH: Đọc/Chạy, bắt đầu từ 0x08000000, dung lượng 64KB */
  RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 20K /* Vùng nhớ RAM: Đọc/Ghi/Chạy, bắt đầu từ 0x20000000, dung lượng 20KB */
}

SECTIONS
{
  .text : { *(.isr_vector) *(.text*) } > FLASH /* Chứa bảng vector ngắt và mã chương trình, lưu vào FLASH */
  .data : { *(.data*) } > RAM AT > FLASH       /* Chứa biến toàn cục có khởi tạo: chạy trên RAM, lưu ban đầu ở FLASH */
  .bss  : { *(.bss*) } > RAM                   /* Chứa biến toàn cục chưa khởi tạo (mặc định = 0), chỉ nằm trên RAM */
}