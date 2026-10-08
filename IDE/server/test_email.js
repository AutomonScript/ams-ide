require('dotenv').config();
const nodemailer = require('nodemailer');

async function testEmail() {
  console.log('Testing SMTP connection...');
  console.log(`User: ${process.env.SMTP_USER}`);

  const transporter = nodemailer.createTransport({
    host: process.env.SMTP_HOST || 'smtp.gmail.com',
    port: Number(process.env.SMTP_PORT) || 587,
    secure: process.env.SMTP_SECURE === 'true',
    auth: {
      user: process.env.SMTP_USER,
      pass: process.env.SMTP_PASS,
    },
  });

  try {
    const info = await transporter.sendMail({
      from: process.env.SMTP_FROM || '"AMS Alert Bot" <no-reply@ams.local>',
      to: process.env.SMTP_USER, // sending it to yourself!
      subject: '✅ AMS Alert Bot is Working!',
      text: 'Hello from AMS!\n\nIf you are reading this, your email configuration is working perfectly!',
    });
    console.log('✅ Email sent successfully!');
    console.log('Message ID:', info.messageId);
  } catch (error) {
    console.error('❌ Failed to send email:');
    console.error(error.message);
  }
}

testEmail();
